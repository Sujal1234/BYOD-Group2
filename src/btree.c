#include "btree.h"
#include <string.h>

struct Item {
    int64_t key;
    RowLoc pos;
};

// Forward declarations of helper functions.
static void deleteFromNode(IndexNode* node, int64_t key);
static void free_node_recursive(IndexNode* node);

static IndexNode* createNode() {
    IndexNode* node = calloc(1, sizeof(IndexNode));
    if (node == NULL) {
        perror("Failed to allocate memory for B-Tree Node");
        return NULL;
    }
    // All pointers are already NULL and values are 0 due to calloc.
    return node;
}

int index_find(IndexNode** root, int64_t key, RowLoc* pos) {
    if (root == NULL || *root == NULL) {
        return 1; // Not found
    }
    IndexNode* current = *root;
    while (current != NULL) {
        int i = 0;
        while (i < current->filled && key > current->values[i]->key) {
            i++;
        }
        // Check if the key is found at the current position.
        if (i < current->filled && key == current->values[i]->key) {
            if (pos != NULL) {
                *pos = current->values[i]->pos; // Update RowLoc with the position.
            }
            return 0; // Found
        }
        // If the current node is a leaf, the search ends here.
        if (current->children == 0) {
            return 1; // Not found
        }
        // Otherwise, continue the search in the appropriate child node.
        current = current->child[i];
    }
    return 1; // Not found
}

static void splitChild(IndexNode* parent, int child_idx) {
    // The child to be split, which must be full (2*MIN - 1 keys).
    IndexNode* child_to_split = parent->child[child_idx];
    
    // Create a new node to store the second half of the keys from the split child.
    IndexNode* new_sibling = createNode();
    new_sibling->filled = MIN - 1;

    // Copy the last (MIN - 1) keys from the child_to_split to the new_sibling.
    for (int j = 0; j < MIN - 1; j++) {
        new_sibling->values[j] = child_to_split->values[j + MIN];
    }

    // If the split child is not a leaf, copy its last MIN children to the new_sibling.
    if (child_to_split->children > 0) {
        new_sibling->children = MIN;
        for (int j = 0; j < MIN; j++) {
            new_sibling->child[j] = child_to_split->child[j + MIN];
        }
        child_to_split->children = MIN;
    }

    // Reduce the number of keys in the original child.
    child_to_split->filled = MIN - 1;

    // Make space in the parent for the new child pointer.
    for (int j = parent->filled; j >= child_idx + 1; j--) {
        parent->child[j + 1] = parent->child[j];
    }

    // Link the new sibling to the parent.
    parent->child[child_idx + 1] = new_sibling;
    parent->children++;

    // Make space in the parent for the median key from the split child.
    for (int j = parent->filled - 1; j >= child_idx; j--) {
        parent->values[j + 1] = parent->values[j];
    }

    // Copy the median key from the split child to the parent.
    parent->values[child_idx] = child_to_split->values[MIN - 1];
    parent->filled++;
}

static void index_insert_nonfull(IndexNode* node, int64_t key, RowLoc pos) {
    int i = node->filled - 1;

    // If the node is a leaf, insert the new key here.
    if (node->children == 0) {
        // Find the correct position for the new key and shift existing keys.
        while (i >= 0 && key < node->values[i]->key) {
            node->values[i + 1] = node->values[i];
            i--;
        }

        Item* new_item = malloc(sizeof(Item));
        if (!new_item) {
            perror("Failed to allocate memory for new Item");
            return;
        }
        new_item->key = key;
        new_item->pos = pos;
        
        node->values[i + 1] = new_item;
        node->filled++;
    } else { // If the node is internal.
        // Find the child that is going to be the root of the new subtree.
        while (i >= 0 && key < node->values[i]->key) {
            i--;
        }
        i++;

        // If the found child is full, split it first.
        if (node->child[i]->filled == (2 * MIN - 1)) {
            splitChild(node, i);
            // After splitting, the key might need to go into the new sibling.
            if (key > node->values[i]->key) {
                i++;
            }
        }
        index_insert_nonfull(node->child[i], key, pos);
    }
}

/**
 * @brief Inserts a new key-position pair into the B-Tree.
 * @param root Pointer to the root of the tree.
 * @param key The key to insert.
 * @param pos The RowLoc associated with the key.
 */
void index_insert(IndexNode** root, int64_t key, RowLoc pos) {
    IndexNode* r = *root;

    // If the tree is empty, create a new root.
    if (r == NULL) {
        *root = createNode();
        Item* new_item = malloc(sizeof(Item));
        if (!new_item) {
             perror("Failed to allocate memory for new Item");
             return;
        }
        new_item->key = key;
        new_item->pos = pos;
        (*root)->values[0] = new_item;
        (*root)->filled = 1;
        return;
    }

    // If the root is full, the tree must grow in height.
    if (r->filled == (2 * MIN - 1)) {
        IndexNode* new_root = createNode();
        *root = new_root;
        new_root->children = 1;
        new_root->child[0] = r;
        splitChild(new_root, 0);
        index_insert_nonfull(new_root, key, pos);
    } else {
        index_insert_nonfull(r, key, pos);
    }
}


// --- Deletion Functions ---

/**
 * @brief Finds the index of the first key >= k.
 */
static int findKey(IndexNode* node, int64_t key) {
    int idx = 0;
    while (idx < node->filled && node->values[idx]->key < key)
        idx++;
    return idx;
}

/**
 * @brief Gets the predecessor of the key at node->values[idx].
 */
static Item* getPredecessor(IndexNode* node, int idx) {
    IndexNode* cur = node->child[idx];
    while (cur->children != 0) {
        cur = cur->child[cur->filled];
    }
    return cur->values[cur->filled - 1];
}

/**
 * @brief Gets the successor of the key at node->values[idx].
 */
static Item* getSuccessor(IndexNode* node, int idx) {
    IndexNode* cur = node->child[idx + 1];
    while (cur->children != 0) {
        cur = cur->child[0];
    }
    return cur->values[0];
}

/**
 * @brief Borrows a key from the previous sibling.
 */
static void borrowFromPrev(IndexNode* node, int idx) {
    IndexNode* child = node->child[idx];
    IndexNode* sibling = node->child[idx - 1];

    for (int i = child->filled - 1; i >= 0; i--)
        child->values[i + 1] = child->values[i];

    if (child->children != 0) {
        for (int i = child->children -1; i >= 0; i--)
            child->child[i + 1] = child->child[i];
    }

    child->values[0] = node->values[idx - 1];

    if (child->children != 0) {
        child->child[0] = sibling->child[sibling->children - 1];
        sibling->children--;
    }

    node->values[idx - 1] = sibling->values[sibling->filled - 1];

    child->filled++;
    if (child->children != 0) child->children++;
    sibling->filled--;
}

/**
 * @brief Borrows a key from the next sibling.
 */
static void borrowFromNext(IndexNode* node, int idx) {
    IndexNode* child = node->child[idx];
    IndexNode* sibling = node->child[idx + 1];

    child->values[child->filled] = node->values[idx];

    if (child->children != 0) {
        child->child[child->children] = sibling->child[0];
    }

    node->values[idx] = sibling->values[0];

    for (int i = 1; i < sibling->filled; i++)
        sibling->values[i - 1] = sibling->values[i];

    if (sibling->children != 0) {
        for (int i = 1; i < sibling->children; i++)
            sibling->child[i - 1] = sibling->child[i];
    }
    
    child->filled++;
    if (child->children != 0) child->children++;
    sibling->filled--;
    if (sibling->children != 0) sibling->children--;
}

/**
 * @brief Merges child[idx] with child[idx+1].
 */
static void mergeNodes(IndexNode* node, int idx) {
    IndexNode* left_child = node->child[idx];
    IndexNode* right_child = node->child[idx + 1];

    left_child->values[left_child->filled] = node->values[idx];

    for (int i = 0; i < right_child->filled; i++)
        left_child->values[left_child->filled + 1 + i] = right_child->values[i];

    if (right_child->children > 0) {
        for (int i = 0; i < right_child->children; i++)
            left_child->child[left_child->filled + 1 + i] = right_child->child[i];
    }

    left_child->filled += 1 + right_child->filled;
    if (right_child->children > 0) {
      left_child->children += right_child->children;
    }


    for (int i = idx + 1; i < node->filled; i++)
        node->values[i - 1] = node->values[i];

    for (int i = idx + 2; i <= node->filled; i++)
        node->child[i - 1] = node->child[i];

    node->filled--;
    node->children--;
    free(right_child);
}

/**
 * @brief Fills a child node if it has fewer than MIN keys.
 */
static void fillNode(IndexNode* node, int idx) {
    if (idx != 0 && node->child[idx - 1]->filled >= MIN)
        borrowFromPrev(node, idx);
    else if (idx != node->filled && node->child[idx + 1]->filled >= MIN)
        borrowFromNext(node, idx);
    else {
        if (idx != node->filled)
            mergeNodes(node, idx);
        else
            mergeNodes(node, idx - 1);
    }
}

/**
 * @brief Recursively deletes a key from a node.
 */
static void deleteFromNode(IndexNode* node, int64_t key) {
    int idx = findKey(node, key);

    if (idx < node->filled && node->values[idx]->key == key) { // Key is in this node
        if (node->children == 0) { // Node is a leaf
            free(node->values[idx]);
            for (int i = idx + 1; i < node->filled; i++)
                node->values[i - 1] = node->values[i];
            node->filled--;
        } else { // Node is internal
            if (node->child[idx]->filled >= MIN) {
                Item* pred = getPredecessor(node, idx);
                node->values[idx]->key = pred->key;
                node->values[idx]->pos = pred->pos;
                deleteFromNode(node->child[idx], pred->key);
            } else if (node->child[idx + 1]->filled >= MIN) {
                Item* succ = getSuccessor(node, idx);
                node->values[idx]->key = succ->key;
                node->values[idx]->pos = succ->pos;
                deleteFromNode(node->child[idx + 1], succ->key);
            } else {
                mergeNodes(node, idx);
                deleteFromNode(node->child[idx], key);
            }
        }
    } else { // Key is not in this node
        if (node->children == 0) {
            // Key not found, should not happen if we check existence before calling
            return;
        }

        bool flag = (idx == node->filled);
        if (node->child[idx]->filled < MIN) {
            fillNode(node, idx);
        }

        if (flag && idx > node->filled)
            deleteFromNode(node->child[idx - 1], key);
        else
            deleteFromNode(node->child[idx], key);
    }
}

/**
 * @brief Deletes a key from the B-Tree.
 * @param root Pointer to the root of the tree.
 * @param key The key to delete.
 */
void index_delete(IndexNode** root, int64_t key) {
    if (root == NULL || *root == NULL) {
        return;
    }

    deleteFromNode(*root, key);

    if ((*root)->filled == 0) {
        IndexNode* tmp = *root;
        if (tmp->children == 0) {
            free(tmp);
            *root = NULL;
        } else {
            *root = tmp->child[0];
            free(tmp);
        }
    }
}

/**
 * @brief Frees the entire B-Tree.
 * @param root Pointer to the root of the tree.
 */
void free_index(IndexNode** root) {
    if (root == NULL || *root == NULL) {
        return;
    }
    IndexNode* node = *root;

    // Recurse for all children
    if (node->children > 0) {
        for (int i = 0; i < node->children; i++) {
            free_index(&(node->child[i]));
        }
    }

    // Free all items in this node
    for (int i = 0; i < node->filled; i++) {
        free(node->values[i]);
    }

    // Free the node itself
    free(node);
    *root = NULL;
}
