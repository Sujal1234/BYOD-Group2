#include "tree.h"

int height(IndexNode *n) {
    if (n == NULL) {
        return 0;
    } else {
        return n->height;
    }
}

int balance_factor(IndexNode *n) {
    if (n == NULL) {
        return 0;
    }
    return height(n->left) - height(n->right);
}

// Helper function to create a new Node
static IndexNode* create_index_node(int64_t key, int page, int slot) {
    IndexNode* node = (IndexNode*)malloc(sizeof(IndexNode));
    node->key = key;
    node->page_index = page;
    node->slot_index = slot;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Right rotate
static IndexNode* right_rotate(IndexNode* y) {
    IndexNode* x = y->left;
    IndexNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = fmax(height(y->left), height(y->right)) + 1;
    x->height = fmax(height(x->left), height(x->right)) + 1;
    return x;
}

// Left rotate
static IndexNode* left_rotate(IndexNode* x) {
    IndexNode* y = x->right;
    IndexNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = fmax(height(x->left), height(x->right)) + 1;
    y->height = fmax(height(y->left), height(y->right)) + 1;
    return y;
}

// Recursive insert
static IndexNode* avl_insert(IndexNode* node, int64_t key, int page, int slot) {
    if (node == NULL)
        return create_index_node(key, page, slot);
    if (key < node->key)
        node->left = avl_insert(node->left, key, page, slot);
    else if (key > node->key)
        node->right = avl_insert(node->right, key, page, slot);
    else
        return node; // Duplicates not allowed
    node->height = 1 + fmax(height(node->left), height(node->right));
    int balance = balance_factor(node);
    // Left Left
    if (balance > 1 && key < node->left->key)
        return right_rotate(node);
    // Right Right
    if (balance < -1 && key > node->right->key)
        return left_rotate(node);
    // Left Right
    if (balance > 1 && key > node->left->key) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    // Right Left
    if (balance < -1 && key < node->right->key) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }
    return node;
}

// Root of the AVL tree
static IndexNode* root = NULL;

void index_insert(int64_t key, int page, int slot) {
    root = avl_insert(root, key, page, slot);
} 




bool index_find(int64_t key, int *page, int *slot){
    IndexNode* curr =root;

    while(curr != NULL){
        if(curr->key == key){
            *page = curr-> page_index;
            *slot = curr -> slot_index;
            return true;
        } else if(key < curr->key){
            curr = curr->left;
        } else{
            curr = curr ->right;
        }
    }
    return false;

}