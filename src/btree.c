#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 4
#define SIZE 100
const int MIN = N/2;

typedef struct Item {
    char* value;  // store a dynamically allocated copy
    int key;
} Item;

typedef struct Node {
    Item* values[N+1]; // N+1 to hold extra item during split
    struct Node* child[N+2];
    int filled;
    int children;
} Node;

Node* createNode() {
    Node* node = (Node*)malloc(sizeof(Node));
    for (int i = 0; i < N+1; i++) {
        node->values[i] = NULL;
        node->child[i] = NULL;
    }
    node->child[N+1] = NULL;
    node->filled = 0;
    node->children = 0;
    return node;
}

const char* search(Node* root, int key) {
    if (root == NULL) return NULL;
    int i = 0;
    while (i < root->filled && key > root->values[i]->key) i++;
    if (i < root->filled && key == root->values[i]->key) {
        return root->values[i]->value;
    } else if (root->child[i] != NULL) {
        return search(root->child[i], key);
    } else {
        return NULL;
    }
}

Node* splitChild(Node* child, Node* parent) {
    int index = 0;
    if (parent == NULL) {
        parent = createNode();
        parent->children = 1;
        parent->child[0] = child;
    } else {
        while (parent->child[index] && parent->child[index]->values[0]->key < child->values[0]->key)
            index++;
    }
    if (child->filled < 2*MIN + 1) {
        printf("Cannot split child, not enough filled items.\n");
        return parent;
    }
    Node* newChild = createNode();
    newChild->filled = MIN;
    for (int j = 0; j < MIN; j++) {
        newChild->values[j] = child->values[j + MIN + 1];
    }
    if (child->children > 0) {
        for (int j = 0; j < MIN + 1; j++) {
            newChild->child[j] = child->child[j + MIN + 1];
        }
        newChild->children = MIN + 1;
        child->children = MIN + 1;
    }
    child->filled -= (MIN + 1);
    for (int j = parent->filled; j > index; j--) {
        parent->values[j] = parent->values[j - 1];
        parent->child[j + 1] = parent->child[j];
    }
    parent->values[index] = child->values[MIN];
    parent->child[index + 1] = newChild;
    parent->filled++;
    parent->children++;
    return parent;
}

Node* insert(Node* root, Node* parent, const char* value, int key) {
    if (search(root, key) != NULL) {
        printf("This key already exists...\n");
        return root;
    }
    if (root == NULL) {
        root = createNode();
        Item* newItem = (Item*)malloc(sizeof(Item));
        newItem->value = strdup(value);
        newItem->key = key;
        root->values[0] = newItem;
        root->filled = 1;
        root->children = 0;
        return root;
    }
    if (root->children == 0) {
        int i = 0;
        while (i < root->filled && root->values[i]->key < key) i++;
        Item* newItem = (Item*)malloc(sizeof(Item));
        newItem->value = strdup(value);
        newItem->key = key;
        for (int j = root->filled; j > i; j--) {
            root->values[j] = root->values[j - 1];
        }
        root->values[i] = newItem;
        root->filled++;
        if (root->filled > N) return splitChild(root, parent);
        return root;
    } else {
        int i = 0;
        while (i < root->filled && root->values[i]->key < key) i++;
        insert(root->child[i], root, value, key);
        if (root->filled > N) return splitChild(root, parent);
        return root;
    }
}

static int findKey(Node* node, int key) {
    int idx = 0;
    while (idx < node->filled && node->values[idx]->key < key)
        idx++;
    return idx;
}

static Item* getPredecessor(Node* node, int idx) {
    Node* cur = node->child[idx];
    while (cur->children != 0) {
        cur = cur->child[cur->filled];
    }
    return cur->values[cur->filled - 1];
}

static Item* getSuccessor(Node* node, int idx) {
    Node* cur = node->child[idx + 1];
    while (cur->children != 0) {
        cur = cur->child[0];
    }
    return cur->values[0];
}

static void borrowFromPrev(Node* node, int idx) {
    Node* child = node->child[idx];
    Node* sibling = node->child[idx - 1];
    for (int i = child->filled - 1; i >= 0; i--) {
        child->values[i + 1] = child->values[i];
    }
    if (child->children != 0) {
        for (int i = child->children - 1; i >= 0; i--) {
            child->child[i + 1] = child->child[i];
        }
    }
    child->values[0] = node->values[idx - 1];
    if (sibling->children != 0) {
        child->child[0] = sibling->child[sibling->children - 1];
    }
    node->values[idx - 1] = sibling->values[sibling->filled - 1];
    child->filled++;
    if (child->children != 0) child->children++;
    sibling->filled--;
    if (sibling->children != 0) sibling->children--;
}

static void borrowFromNext(Node* node, int idx) {
    Node* child = node->child[idx];
    Node* sibling = node->child[idx + 1];
    child->values[child->filled] = node->values[idx];
    if (sibling->children != 0) {
        child->child[child->filled + 1] = sibling->child[0];
    }
    child->filled++;
    if (child->children != 0) child->children++;
    node->values[idx] = sibling->values[0];
    for (int i = 1; i < sibling->filled; i++) {
        sibling->values[i - 1] = sibling->values[i];
    }
    if (sibling->children != 0) {
        for (int i = 1; i < sibling->children; i++) {
            sibling->child[i - 1] = sibling->child[i];
        }
    }
    sibling->filled--;
    if (sibling->children != 0) sibling->children--;
}

static void mergeNodes(Node* node, int idx) {
    Node* left = node->child[idx];
    Node* right = node->child[idx + 1];
    left->values[left->filled] = node->values[idx];
    for (int i = 0; i < right->filled; i++) {
        left->values[left->filled + 1 + i] = right->values[i];
    }
    if (right->children != 0) {
        for (int i = 0; i < right->children; i++) {
            left->child[left->filled + 1 + i] = right->child[i];
        }
        left->children = left->filled + 1 + right->children;
    }
    left->filled += 1 + right->filled;
    for (int i = idx + 1; i < node->filled; i++) {
        node->values[i - 1] = node->values[i];
    }
    for (int i = idx + 2; i <= node->filled; i++) {
        node->child[i - 1] = node->child[i];
    }
    node->filled--;
    node->children--;
    free(right);
}

static void fillNode(Node* node, int idx) {
    if (idx != 0 && node->child[idx - 1]->filled >= MIN + 1) {
        borrowFromPrev(node, idx);
    } else if (idx != node->filled && node->child[idx + 1]->filled >= MIN + 1) {
        borrowFromNext(node, idx);
    } else {
        if (idx != node->filled) {
            mergeNodes(node, idx);
        } else {
            mergeNodes(node, idx - 1);
        }
    }
}

static void deleteFromNode(Node* node, int key) {
    int idx = findKey(node, key);
    if (idx < node->filled && node->values[idx]->key == key) {
        if (node->children == 0) {
            free(node->values[idx]->value);
            free(node->values[idx]);
            for (int i = idx + 1; i < node->filled; i++) {
                node->values[i - 1] = node->values[i];
            }
            node->filled--;
            return;
        }
        if (node->child[idx]->filled >= MIN + 1) {
            Item* pred = getPredecessor(node, idx);
            node->values[idx]->key = pred->key;
            free(node->values[idx]->value);
            node->values[idx]->value = strdup(pred->value);
            deleteFromNode(node->child[idx], pred->key);
        } else if (node->child[idx + 1]->filled >= MIN + 1) {
            Item* succ = getSuccessor(node, idx);
            node->values[idx]->key = succ->key;
            free(node->values[idx]->value);
            node->values[idx]->value = strdup(succ->value);
            deleteFromNode(node->child[idx + 1], succ->key);
        } else {
            mergeNodes(node, idx);
            deleteFromNode(node->child[idx], key);
        }
        return;
    }
    if (node->children == 0) {
        printf("Key %d not found in the tree.\n", key);
        return;
    }
    int flag = (idx == node->filled);
    if (node->child[idx]->filled < MIN + 1) {
        fillNode(node, idx);
    }
    if (flag && idx > node->filled) {
        deleteFromNode(node->child[idx - 1], key);
    } else {
        deleteFromNode(node->child[idx], key);
    }
}

void deleteNode(Node** rootPtr, int key) {
    if (*rootPtr == NULL) {
        printf("Tree is empty.\n");
        return;
    }
    if (search(*rootPtr, key) == NULL) {
        printf("There is no such key to delete...\n");
        return;
    }
    deleteFromNode(*rootPtr, key);
    if ((*rootPtr)->filled == 0) {
        Node* tmp = *rootPtr;
        if (tmp->children == 0) {
            free(tmp);
            *rootPtr = NULL;
        } else {
            *rootPtr = tmp->child[0];
            free(tmp);
        }
    }
}

int printLevel(Node* root, int level) {
    if (root == NULL) return 0;
    if (level == 0) {
        int count = 0;
        for (int i = 0; i < root->filled; i++) {
            printf("Key: %d  Value: %s || ", root->values[i]->key, root->values[i]->value);
            count++;
        }
        printf("\n");
        return count;
    } else {
        int total = 0;
        // Only traverse the first ‘children’ pointers, which truly exist
        for (int i = 0; i < root->children; i++) {
            total += printLevel(root->child[i], level - 1);
        }
        return total;
    }
}

void printTree(Node* root) {
    if (root == NULL) {
        printf("(empty tree)\n");
        return;
    }
    int level = 0;
    int nodes;
    do {
        printf("\nDepth: %d\n", level);
        nodes = printLevel(root, level);
        level++;
    } while (nodes > 0);
}

int main() {
    int keys = 0;
    size_t bufsize = 200;
    char buffer[200];

    printf("Keep inputting strings, which will be stored in the B-tree... type \"!print\" to print the tree... Input empty string to end...\n");
    Node* root = NULL;
    while (fgets(buffer, bufsize, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        if(strlen(buffer) == 0) {
            break; // end if empty input
        }
        if (strcmp(buffer, "!print") == 0) {
            printTree(root);
        } else {
            root = insert(root, NULL, buffer, keys++);
        }
    }

    printTree(root);
    printf("\nNow keep entering the keys you would like to delete... Use -1 to print tree and -2 to stop\n");

    int inp = 0;
    while (inp != -2) {
        scanf("%d", &inp);
        if (inp == -1) {
            printTree(root);
        } else if (inp == -2) {
            break;
        } else {
            deleteNode(&root, inp);
        }
    }
    return 0;
}