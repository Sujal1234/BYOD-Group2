#ifndef BTREE_H
#define BTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// This B-Tree implementation is a simplified version that does Linear Search(not Binary), stores the B-Tree in Ram, not disk and is meant for educational purposes.
// This API uses the same function names as AVL tree API, to be used as a drop-in replacement
#include "page.h" // For RowLoc
#define N 4 // Degree of the B-Tree, i.e., maximum number of children per node
#define MIN (N/2) // Minimum number of keys in a non-root node

struct Item;           // Forward declaration
typedef struct Item Item; // Typedef alias

typedef struct IndexNode {
    Item* values[N+1];
    struct IndexNode* child[N+2];
    int filled;
    int children;
} IndexNode;

// can make the index and insert return, but not needed for now
void index_insert(IndexNode** root, int64_t key, RowLoc pos); // Inserts a new node with key and position into the AVL tree
int index_find(IndexNode** root, int64_t key, RowLoc* pos); // Finds the node with the given key and updates pos with its position, returns 0 if found, 1 if not found
void index_delete(IndexNode** root, int64_t key); // Deletes the node with the given key from the AVL tree
void free_index(IndexNode** root); // Frees the AVL tree

#endif //BTREE_H