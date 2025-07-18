#ifndef TREE_H
#define TREE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


typedef struct IndexNode {
    int64_t key;
    int page_index;
    int slot_index;
    int height;
    struct IndexNode* left;
    struct IndexNode* right;
} IndexNode;

int height(IndexNode * n);

int balance_factor(IndexNode * n);  // avl balance = 0,-1,1

void index_insert(int64_t key, int page, int slot);
bool index_find(int64_t key, int *page, int *slot);
void index_delete(int64_t key);


#endif //TREE_H