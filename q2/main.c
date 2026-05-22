#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_NAME 64
#define LINE_BUF 128

typedef struct Stop {
    char name[MAX_NAME];
    int number;
    struct Stop* prev;
    struct Stop* next;
} Stop;

typedef struct {
    Stop* head;
    Stop* tail;
} Route;

Stop* createStop(const char* name, int number) {
    Stop* s = (Stop*)malloc(sizeof(Stop));
    if (s == NULL) {
        fprintf(stderr, "Error: out of memory.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(s->name, name, MAX_NAME - 1);
    s->name[MAX_NAME - 1] = '\0';
    s->number = number;
    s->prev = NULL;
    s->next = NULL;
    return s;
}

// O(1) append because we keep a tail pointer.
void appendStop(Route* r, const char* name, int number) {
    Stop* s = createStop(name, number);
    if (r->head == NULL) {
        r->head = s;
        r->tail = s;
    } else {
        s->prev = r->tail;
        r->tail->next = s;
        r->tail = s;
    }
}

void traverseForward(Route* r) {
    if (r->head == NULL) {
        printf("Route is empty.\n");
        return;
    }
    printf("\n--- Forward (home -> campus) ---\n");
    for (Stop* c = r->head; c != NULL; c = c->next) {
        printf("Stop %d: %s\n", c->number, c->name);
        fflush(stdout);
        if (c->next != NULL) sleep(3);
    }
    printf("Arrived at campus.\n");
}

void traverseBackward(Route* r) {
    if (r->tail == NULL) {
        printf("Route is empty.\n");
        return;
    }
    printf("\n--- Backward (campus -> home) ---\n");
    for (Stop* c = r->tail; c != NULL; c = c->prev) {
        printf("Stop %d: %s\n", c->number, c->name);
        fflush(stdout);
        if (c->prev != NULL) sleep(3);
    }
    printf("Arrived at home.\n");
}

void showRoute(Route* r) {
    if (r->head == NULL) {
        printf("Route is empty.\n");
        return;
    }
    printf("Route: ");
    for (Stop* c = r->head; c != NULL; c = c->next) {
        printf("[%d:%s]%s", c->number, c->name, c->next ? " <-> " : "\n");
    }
}

void freeRoute(Route* r) {
    Stop* c = r->head;
    while (c != NULL) {
        Stop* next = c->next;
        free(c);
        c = next;
    }
    r->head = NULL;
    r->tail = NULL;
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

void promptAddStop(Route* r) {
    char name[MAX_NAME];
    int num;
    printf("Enter stop name: ");
    fflush(stdout);
    if (!readLine(name, sizeof(name)) || name[0] == '\0') {
        printf("Empty name; cancelled.\n");
        return;
    }
    if (!readInt("Enter stop number: ", &num)) return;
    appendStop(r, name, num);
    printf("Added stop %d (%s) at end of route.\n", num, name);
}

int main(void) {
    Route route = { NULL, NULL };
    int initialCount = 0;

    if (!readInt("How many initial bus stops? ", &initialCount)) return 0;
    if (initialCount < 0) initialCount = 0;

    for (int i = 0; i < initialCount; i++) {
        char name[MAX_NAME];
        int num;
        printf("\nStop %d of %d\n", i + 1, initialCount);
        printf("  Name: ");
        fflush(stdout);
        if (!readLine(name, sizeof(name)) || name[0] == '\0') {
            printf("  Empty name; please try again.\n");
            i--;
            continue;
        }
        if (!readInt("  Number: ", &num)) {
            freeRoute(&route);
            return 0;
        }
        appendStop(&route, name, num);
    }

    for (;;) {
        printf("\n=== Menu ===\n");
        printf("1) Move forward (home -> campus)\n");
        printf("2) Move backward (campus -> home)\n");
        printf("3) Add a new stop at the end\n");
        printf("4) Show the route\n");
        printf("5) Quit\n");
        int choice;
        if (!readInt("Choose: ", &choice)) break;
        switch (choice) {
            case 1: traverseForward(&route); break;
            case 2: traverseBackward(&route); break;
            case 3: promptAddStop(&route); break;
            case 4: showRoute(&route); break;
            case 5:
                freeRoute(&route);
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    freeRoute(&route);
    return 0;
}
