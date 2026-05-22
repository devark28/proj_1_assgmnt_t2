#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUF 64

typedef struct TreeNode {
    int value;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

typedef struct QueueNode {
    TreeNode* tree;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* head;
    QueueNode* tail;
} Queue;

void qInit(Queue* q) {
    q->head = NULL;
    q->tail = NULL;
}

void qPush(Queue* q, TreeNode* t) {
    QueueNode* n = (QueueNode*)malloc(sizeof(QueueNode));
    if (n == NULL) { fprintf(stderr, "Error: out of memory.\n"); exit(EXIT_FAILURE); }
    n->tree = t;
    n->next = NULL;
    if (q->tail == NULL) {
        q->head = n;
        q->tail = n;
    } else {
        q->tail->next = n;
        q->tail = n;
    }
}

TreeNode* qPop(Queue* q) {
    if (q->head == NULL) return NULL;
    QueueNode* n = q->head;
    TreeNode* t = n->tree;
    q->head = n->next;
    if (q->head == NULL) q->tail = NULL;
    free(n);
    return t;
}

int qEmpty(Queue* q) { return q->head == NULL; }

TreeNode* createNode(int value) {
    TreeNode* n = (TreeNode*)malloc(sizeof(TreeNode));
    if (n == NULL) { fprintf(stderr, "Error: out of memory.\n"); exit(EXIT_FAILURE); }
    n->value = value;
    n->left = NULL;
    n->right = NULL;
    return n;
}

// Insertion rule: level-order (top-to-bottom, left-to-right).
// arr[0] becomes the root; each next value becomes the next empty slot
// in breadth-first order. Result is a complete binary tree.
TreeNode* buildLevelOrder(const int* arr, int n) {
    if (n <= 0) return NULL;
    TreeNode* root = createNode(arr[0]);
    Queue q; qInit(&q);
    qPush(&q, root);
    int i = 1;
    while (i < n && !qEmpty(&q)) {
        TreeNode* parent = qPop(&q);
        if (i < n) {
            parent->left = createNode(arr[i++]);
            qPush(&q, parent->left);
        }
        if (i < n) {
            parent->right = createNode(arr[i++]);
            qPush(&q, parent->right);
        }
    }
    while (!qEmpty(&q)) qPop(&q);
    return root;
}

void freeTree(TreeNode* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

TreeNode* findNode(TreeNode* root, int v) {
    if (root == NULL) return NULL;
    if (root->value == v) return root;
    TreeNode* l = findNode(root->left, v);
    if (l != NULL) return l;
    return findNode(root->right, v);
}

TreeNode* findParent(TreeNode* root, int v) {
    if (root == NULL) return NULL;
    if ((root->left  && root->left->value  == v) ||
        (root->right && root->right->value == v)) {
        return root;
    }
    TreeNode* l = findParent(root->left, v);
    if (l != NULL) return l;
    return findParent(root->right, v);
}

void printLeaves(TreeNode* root) {
    if (root == NULL) return;
    if (root->left == NULL && root->right == NULL) {
        printf("%d ", root->value);
        return;
    }
    printLeaves(root->left);
    printLeaves(root->right);
}

void printSiblings(TreeNode* root, int v) {
    if (root != NULL && root->value == v) {
        printf("(none -- %d is the root)\n", v);
        return;
    }
    TreeNode* p = findParent(root, v);
    if (p == NULL) { printf("(node not found)\n"); return; }
    int found = 0;
    if (p->left  && p->left->value  != v) { printf("%d ", p->left->value);  found = 1; }
    if (p->right && p->right->value != v) { printf("%d ", p->right->value); found = 1; }
    if (!found) printf("(none)");
    printf("\n");
}

void printParent(TreeNode* root, int v) {
    if (root != NULL && root->value == v) {
        printf("(none -- %d is the root)\n", v);
        return;
    }
    TreeNode* p = findParent(root, v);
    if (p == NULL) printf("(node not found)\n");
    else printf("%d\n", p->value);
}

void printGrandchildren(TreeNode* root, int v) {
    TreeNode* node = findNode(root, v);
    if (node == NULL) { printf("(node not found)\n"); return; }
    TreeNode* kids[2] = { node->left, node->right };
    int found = 0;
    for (int i = 0; i < 2; i++) {
        if (kids[i] == NULL) continue;
        if (kids[i]->left)  { printf("%d ", kids[i]->left->value);  found = 1; }
        if (kids[i]->right) { printf("%d ", kids[i]->right->value); found = 1; }
    }
    if (!found) printf("(none)");
    printf("\n");
}

int readLine(char* buf, int size) {
    if (fgets(buf, size, stdin) == NULL) return 0;
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
    return 1;
}

int readInt(const char* prompt, int* out) {
    char buf[LINE_BUF];
    for (;;) {
        printf("%s", prompt);
        fflush(stdout);
        if (!readLine(buf, sizeof(buf))) return 0;
        if (sscanf(buf, "%d", out) == 1) return 1;
        printf("Please enter a valid integer.\n");
    }
}

int main(void) {
    int arr[] = {
        17, 42, 88,  3, 91, 56, 12, 73, 28, 64,
         9, 81, 35, 67, 22, 99, 14, 53, 77, 31,
        46, 85,  7, 60, 25, 70, 38, 92, 19, 50,
        83, 11, 58, 41, 95, 26, 75, 33, 87, 16,
        62, 49, 78, 21, 94, 36, 80, 13, 55, 96,
        24, 71, 30, 89, 18,100, 39, 86,  5, 65,
        27, 74, 32, 90, 20, 79,  8, 84
    };
    int n = (int)(sizeof(arr) / sizeof(arr[0]));

    TreeNode* root = buildLevelOrder(arr, n);
    printf("Built level-order binary tree with %d nodes.\n", n);

    int running = 1;
    while (running) {
        printf("\n=== Menu ===\n");
        printf("1) Print the root\n");
        printf("2) Print all leaf nodes\n");
        printf("3) Query a node (siblings, parent, grandchildren)\n");
        printf("4) Quit\n");
        int choice;
        if (!readInt("Choose: ", &choice)) break;
        switch (choice) {
            case 1:
                printf("Root: %d\n", root->value);
                break;
            case 2:
                printf("Leaves: ");
                printLeaves(root);
                printf("\n");
                break;
            case 3: {
                int v;
                if (!readInt("Enter node value: ", &v)) { running = 0; break; }
                if (findNode(root, v) == NULL) {
                    printf("Node %d does not exist in the tree.\n", v);
                    break;
                }
                printf("Siblings of %d: ", v);     printSiblings(root, v);
                printf("Parent of %d: ", v);       printParent(root, v);
                printf("Grandchildren of %d: ", v); printGrandchildren(root, v);
                break;
            }
            case 4:
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
