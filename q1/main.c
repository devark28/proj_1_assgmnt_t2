#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 64
#define MAX_ID 32
#define MAX_LINE 256

typedef struct {
    char firstName[MAX_NAME];
    char lastName[MAX_NAME];
    char id[MAX_ID];
    int grade;
} Student;

// Ascending by grade; ties broken by last name lexicographically.
int compareStudents(const Student* a, const Student* b) {
    if (a->grade != b->grade) return a->grade - b->grade;
    return strcmp(a->lastName, b->lastName);
}

void swapStudents(Student* a, Student* b) {
    Student tmp = *a;
    *a = *b;
    *b = tmp;
}

// Lomuto partition with the last element as pivot.
int partition(Student arr[], int low, int high) {
    Student pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (compareStudents(&arr[j], &pivot) <= 0) {
            i++;
            swapStudents(&arr[i], &arr[j]);
        }
    }
    swapStudents(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quickSort(Student arr[], int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);
        quickSort(arr, low, p - 1);
        quickSort(arr, p + 1, high);
    }
}

Student* readStudents(const char* path, int* outCount) {
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Error: cannot open input file '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    int cap = 16;
    int n = 0;
    Student* arr = (Student*)malloc(cap * sizeof(Student));
    if (arr == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    int lineNo = 0;
    while (fgets(line, sizeof(line), f) != NULL) {
        lineNo++;
        Student s;
        if (sscanf(line, "%63s %63s %31s %d",
                   s.firstName, s.lastName, s.id, &s.grade) != 4) {
            fprintf(stderr, "Warning: skipping malformed line %d\n", lineNo);
            continue;
        }
        if (s.grade < 0 || s.grade > 100) {
            fprintf(stderr, "Warning: skipping line %d (grade %d out of range)\n",
                    lineNo, s.grade);
            continue;
        }
        if (n == cap) {
            cap *= 2;
            Student* grown = (Student*)realloc(arr, cap * sizeof(Student));
            if (grown == NULL) {
                fprintf(stderr, "Error: memory reallocation failed.\n");
                free(arr);
                fclose(f);
                exit(EXIT_FAILURE);
            }
            arr = grown;
        }
        arr[n++] = s;
    }

    fclose(f);
    *outCount = n;
    return arr;
}

void writeStudents(const char* path, Student arr[], int n) {
    FILE* f = fopen(path, "w");
    if (f == NULL) {
        fprintf(stderr, "Error: cannot write to '%s'\n", path);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        fprintf(f, "%s %s %s %d\n",
                arr[i].firstName, arr[i].lastName, arr[i].id, arr[i].grade);
    }
    fclose(f);
}

int main(void) {
    int n = 0;
    Student* students = readStudents("students.txt", &n);

    if (n == 0) {
        fprintf(stderr, "No valid student records found.\n");
        free(students);
        return EXIT_FAILURE;
    }

    quickSort(students, 0, n - 1);
    writeStudents("sorted_students.txt", students, n);

    printf("Sorted %d student record(s) -> sorted_students.txt\n", n);

    free(students);
    return 0;
}
