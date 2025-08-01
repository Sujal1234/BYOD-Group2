#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "page.h" // For RowLoc

typedef struct IndexNode {
    int64_t key; // row id
    RowLoc pos;
    int height;
    struct IndexNode* left;
    struct IndexNode* right;
} IndexNode;

// can make the index and insert return, but not needed for now
void index_insert(IndexNode** root, int64_t key, RowLoc pos); // Inserts a new node with key and position into the AVL tree
int index_find(IndexNode** root, int64_t key, RowLoc* pos); // Finds the node with the given key and updates pos with its position, returns 0 if found, 1 if not found
void index_delete(IndexNode** root, int64_t key); // Deletes the node with the given key from the AVL tree
void free_index(IndexNode** root); // Frees the AVL tree

#endif //TREE_H