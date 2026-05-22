#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 64
#define MAX_LINE 256

typedef struct Record {
    char firstName[MAX_NAME];
    int grade;
    struct Record* next;
} Record;

typedef struct BstNode {
    char lastName[MAX_NAME];
    Record* records;
    struct BstNode* left;
    struct BstNode* right;
} BstNode;

char* trim(char* s) {
    while (*s && isspace((unsigned char)*s)) s++;
    char* end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1])) end--;
    *end = '\0';
    return s;
}

Record* createRecord(const char* firstName, int grade) {
    Record* r = (Record*)malloc(sizeof(Record));
    if (r == NULL) { fprintf(stderr, "Error: out of memory.\n"); exit(EXIT_FAILURE); }
    strncpy(r->firstName, firstName, MAX_NAME - 1);
    r->firstName[MAX_NAME - 1] = '\0';
    r->grade = grade;
    r->next = NULL;
    return r;
}

BstNode* createNode(const char* lastName, const char* firstName, int grade) {
    BstNode* n = (BstNode*)malloc(sizeof(BstNode));
    if (n == NULL) { fprintf(stderr, "Error: out of memory.\n"); exit(EXIT_FAILURE); }
    strncpy(n->lastName, lastName, MAX_NAME - 1);
    n->lastName[MAX_NAME - 1] = '\0';
    n->records = createRecord(firstName, grade);
    n->left = NULL;
    n->right = NULL;
    return n;
}

// Duplicate last names share a node; their records are pushed onto an internal list.
BstNode* insert(BstNode* root, const char* lastName, const char* firstName, int grade) {
    if (root == NULL) return createNode(lastName, firstName, grade);
    int cmp = strcmp(lastName, root->lastName);
    if (cmp < 0)      root->left  = insert(root->left,  lastName, firstName, grade);
    else if (cmp > 0) root->right = insert(root->right, lastName, firstName, grade);
    else {
        Record* r = createRecord(firstName, grade);
        r->next = root->records;
        root->records = r;
    }
    return root;
}

BstNode* search(BstNode* root, const char* lastName) {
    while (root != NULL) {
        int cmp = strcmp(lastName, root->lastName);
        if (cmp == 0) return root;
        root = (cmp < 0) ? root->left : root->right;
    }
    return NULL;
}

void freeRecords(Record* r) {
    while (r != NULL) {
        Record* next = r->next;
        free(r);
        r = next;
    }
}

void freeTree(BstNode* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    freeRecords(root->records);
    free(root);
}

int loadFromFile(const char* path, BstNode** root) {
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Error: cannot open '%s'\n", path);
        return -1;
    }
    int loaded = 0;
    int lineNo = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f) != NULL) {
        lineNo++;
        char* p = strchr(line, '\n'); if (p) *p = '\0';
        if (strchr(line, '|') == NULL) continue;

        char* t1 = strtok(line, "|");
        char* t2 = strtok(NULL, "|");
        char* t3 = strtok(NULL, "|");
        if (t1 == NULL || t2 == NULL || t3 == NULL) {
            fprintf(stderr, "Warning: skipping malformed line %d\n", lineNo);
            continue;
        }
        char* fn = trim(t1);
        char* ln = trim(t2);
        char* gs = trim(t3);
        int grade;
        if (*fn == '\0' || *ln == '\0' || sscanf(gs, "%d", &grade) != 1
            || grade < 0 || grade > 100) {
            fprintf(stderr, "Warning: skipping malformed line %d\n", lineNo);
            continue;
        }
        *root = insert(*root, ln, fn, grade);
        loaded++;
    }
    fclose(f);
    return loaded;
}

void printMatches(const char* lastName, BstNode* node) {
    if (node == NULL) {
        printf("No record found for last name '%s'.\n", lastName);
        return;
    }
    int n = 0;
    for (Record* r = node->records; r != NULL; r = r->next) n++;
    printf("Found %d record(s) for last name '%s':\n", n, lastName);
    for (Record* r = node->records; r != NULL; r = r->next) {
        printf("  %s %s - grade %d\n", r->firstName, node->lastName, r->grade);
    }
}

void inorder(BstNode* root) {
    if (root == NULL) return;
    inorder(root->left);
    for (Record* r = root->records; r != NULL; r = r->next) {
        printf("  %s %s - grade %d\n", r->firstName, root->lastName, r->grade);
    }
    inorder(root->right);
}

int readLine(char* buf, int size) {
    if (fgets(buf, size, stdin) == NULL) return 0;
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
    return 1;
}

int main(void) {
    BstNode* root = NULL;
    int loaded = loadFromFile("students.txt", &root);
    if (loaded <= 0) {
        fprintf(stderr, "No valid student records loaded.\n");
        freeTree(root);
        return EXIT_FAILURE;
    }
    printf("Loaded %d student record(s) into BST.\n", loaded);

    int running = 1;
    while (running) {
        printf("\n=== Menu ===\n");
        printf("1) Search by last name\n");
        printf("2) Print all records (in-order)\n");
        printf("3) Quit\n");
        printf("Choose: ");
        fflush(stdout);

        char buf[MAX_LINE];
        if (!readLine(buf, sizeof(buf))) break;
        int choice;
        if (sscanf(buf, "%d", &choice) != 1) { printf("Invalid choice.\n"); continue; }

        switch (choice) {
            case 1: {
                printf("Enter last name: ");
                fflush(stdout);
                if (!readLine(buf, sizeof(buf))) { running = 0; break; }
                char* key = trim(buf);
                if (*key == '\0') { printf("Empty input.\n"); break; }
                printMatches(key, search(root, key));
                break;
            }
            case 2:
                printf("All records (in-order by last name):\n");
                inorder(root);
                break;
            case 3:
                running = 0;
                break;
            default:
                printf("Invalid choice.\n");
        }
    }

    freeTree(root);
    printf("Goodbye!\n");
    return 0;
}
