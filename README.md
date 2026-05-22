# Project 1 — C Data Structures

Four short C programs for a data structures assignment. Each lives in its own directory and compiles standalone.

## Layout

```
proj_1_assgmnt/
├── q1/     # Quick Sort on student records (file → sort → file)
├── q2/     # Doubly linked list bus route
├── q3/     # Binary tree built from 68 distinct integers
└── q4/     # BST on student records keyed by last name
```

## Requirements

GCC with C11 support, and a POSIX system — Q2 uses `sleep()` from `<unistd.h>`.

## Build & run

Each program is a single source file. From the project root:

```
cd q1 && gcc -Wall -Wextra -std=c11 -o main main.c && ./main
cd q2 && gcc -Wall -Wextra -std=c11 -o main main.c && ./main
cd q3 && gcc -Wall -Wextra -std=c11 -o main main.c && ./main
cd q4 && gcc -Wall -Wextra -std=c11 -o main main.c && ./main
```

Programs that read input files (Q1, Q4) expect the file next to the binary, so `cd` into the question's directory before running.

## Q1 — Quick Sort

Reads `students.txt` next to the binary. Each line has the form:

```
Firstname LastName StudentID Grade
```

Space-separated; `Grade` is an integer in `[0, 100]`. Malformed or out-of-range lines are skipped with a warning to stderr. Sorted output is written to `sorted_students.txt` in the same format.

## Q2 — Doubly Linked List Bus Route

Prompts for the number of initial stops, then their name and number. After setup, a menu offers:

1. Forward traversal (home → campus), 3-second delay between stops
2. Backward traversal (campus → home), 3-second delay between stops
3. Add a new stop at the end of the route
4. Show the full route
5. Quit

## Q3 — Binary Tree

The 68 distinct integers are hardcoded in `main.c`. The tree is built with **level-order (BFS) insertion**, producing a complete binary tree. Menu:

1. Print the root
2. Print all leaf nodes
3. Query a node for its siblings, parent, and grandchildren
4. Quit

## Q4 — Binary Search Tree on Last Name

Reads `students.txt` next to the binary. Each line has the form:

```
Firstname| Lastname| Grade
```

Pipe-separated; surrounding whitespace is tolerated. Malformed or empty lines are skipped with a warning. Duplicate last names share a single BST node and chain their records on an internal linked list. Menu:

1. Search by last name
2. Print all records (in-order)
3. Quit
