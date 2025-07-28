#include "pager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For strncpy

// Double Linked List Node Structure
// This structure is used to implement the LRU Cache.
// Each node in the linked list holds a pointer to a Page and links to prev/next nodes.
typedef struct DLLNode {
    Page* page; // Pointer to the actual Page data
    struct DLLNode *prev;
    struct DLLNode *next;
} DLLNode;

// LRU Cache (implemented without hashmaps for lookups)
typedef struct LRUCache {
    int capacity;
    int current_size;
    DLLNode* head;  // Most recently used (MRU) end
    DLLNode* tail;  // Least recently used (LRU) end
} LRUCache;

int save_page(Page* page, const char* data_dir) {
    if (page == NULL || data_dir == NULL) {
        printf("Invalid arguments to save_page!\n");
        return 1;
    }

    char filename[256];
    snprintf(filename, sizeof(filename), "%s/page_%d.bin", data_dir, page->header.page_id);

    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Failed to open file for saving page!\n");
        return 1;
    }

    size_t written = fwrite(page, sizeof(Page), 1, file);
    fclose(file);

    if (written != 1) {
        printf("Failed to write page to file!\n");
        return 1;
    }
    return 0;
}

Page* load_page(int page_id, const char* data_dir) {
    if (data_dir == NULL) {
        printf("Invalid data directory!\n");
        return NULL;
    }

    char filename[256];
    snprintf(filename, sizeof(filename), "%s/page_%d.bin", data_dir, page_id);

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file for loading page!\n");
        return NULL;
    }

    Page* page = calloc(1, sizeof(Page));
    if (page == NULL) {
        printf("Failed to allocate memory for Page!\n");
        fclose(file);
        return NULL;
    }

    size_t read = fread(page, sizeof(Page), 1, file);
    fclose(file);

    if (read != 1) {
        printf("Failed to read page from file!\n");
        free(page);
        return NULL;
    }
    return page;
}

static DLLNode* create_DLLNode(Page* page) {
    DLLNode* newNode = (DLLNode*)malloc(sizeof(DLLNode));
    if (newNode == NULL) {
        printf("Failed to allocate memory for DLLNode!\n");
        return NULL;
    }
    newNode->page = page;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

// Helper functions
// Add a node to the front of the linked list
static void addNodeToFront(LRUCache* cache, DLLNode* node) {
    node->next = cache->head;
    node->prev = NULL;

    if (cache->head != NULL) {
        cache->head->prev = node;
    }
    cache->head = node;

    if (cache->tail == NULL) { // If list was empty, this is also the tail
        cache->tail = node;
    }
}

// Remove a node from anywhere in the linked list
static void removeNode(LRUCache* cache, DLLNode* node) {
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else { // Node is the head
        cache->head = node->next;
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else { // Node is the tail
        cache->tail = node->prev;
    }
    node->prev = NULL;
    node->next = NULL;
    //free(node); // Free the node itself, not used, as we free it in LRUCache_put
}

// LRU functions
static LRUCache* create_LRUCache() {
    LRUCache* cache = calloc(1, sizeof(LRUCache));
    if (cache == NULL) {
        printf("Failed to allocate memory for LRUCache!\n");
        return NULL;
    }
    cache->capacity = CACHE_SIZE;
    cache->current_size = 0;
    cache->head = NULL;
    cache->tail = NULL;
    return cache;
}

// Get a page from the cache
static Page* LRUCache_get(LRUCache* cache, int page_id) {
    DLLNode* current = cache->head;
    while (current != NULL) {
        if (current->page->header.page_id == page_id) {
            // Page found: move its node to the front (MRU) of the linked list
            if (current != cache->head) { // Only move if it's not already the head
                removeNode(cache, current);
                addNodeToFront(cache, current);
            }
            printf("Cache Hit: Page %d accessed. Moved to MRU.\n", page_id);
            return current->page;
        }
        current = current->next;
    }

    printf("Cache Miss: Page %d not found.\n", page_id);
    return NULL; // Page not in cache
}

// Put a page into the cache. Used when the get method returns NULL(cache miss), after the pager reads from disk.
// This also updates the page if it already exists in the cache.
static int LRUCache_put(LRUCache* cache, Page* page, const char* data_dir) {
    if (page == NULL) {
        printf("Cannot put a NULL page into the cache!\n");
        return 1;
    }

    // First, check if the page already exists in the cache by traversing the DLL
    DLLNode* existing_node = NULL;
    DLLNode* current = cache->head;
    while (current != NULL) {
        if (current->page->header.page_id == page->header.page_id) {
            existing_node = current;
            break;
        }
        current = current->next;
    }

    if (existing_node != NULL) {
        // Page already exists in cache, page updated
        free_page(existing_node->page);
        existing_node->page = page;

        // Move the existing node to the front (MRU)
        if (existing_node != cache->head) {
            removeNode(cache, existing_node);
            addNodeToFront(cache, existing_node);
        }
        printf("Page %d already in cache. Content updated and moved to MRU.\n", page->header.page_id);
    } else { // new page to be added
        DLLNode* newNode = create_DLLNode(page);

        addNodeToFront(cache, newNode);
        cache->current_size++;

        printf("Page %d added to cache. Current size: %d/%d.\n", page->header.page_id, cache->current_size, cache->capacity);

        // Check for capacity constraints
        if (cache->current_size > cache->capacity) {
            DLLNode* lruNode = cache->tail;
            if (lruNode == NULL) { // Should not happen, but jic
                printf("Cache size mismatch with tail pointer during removal!\n");
                return;
            }
            printf("Cache full. Removing LRU Page %d.\n", lruNode->page->header.page_id);
            removeNode(cache, lruNode);
            save_page(lruNode->page, data_dir); // Save the page to disk before removing it from cache
            free_page(lruNode->page); // Free the actual Page data
            free(lruNode);           // Free the DLLNode
            cache->current_size--;
        }
    }
    return 0;
}

// Free all memory associated with the LRU Cache
static void free_LRUCache(LRUCache* cache) {
    if (cache == NULL) return;
    DLLNode* current_node = cache->head;
    while (current_node != NULL) {
        DLLNode* next_node = current_node->next;
        free_page(current_node->page);
        free(current_node);           
        current_node = next_node;
    }
    cache->head = NULL;
    cache->tail = NULL;

    free(cache);
    printf("LRU Cache freed successfully.\n");
}

Pager* create_pager(const char* data_dir) {
    Pager* pager = calloc(1, sizeof(Pager));
    if (pager == NULL) {
        printf("Failed to allocate memory for Pager!\n");
        return NULL;
    }
    pager->cache = create_LRUCache();
    if (pager->cache == NULL) {
        printf("Failed to allocate LRUCache for Pager!\n");
        free(pager);
        return NULL;
    }
    pager->data_dir = data_dir; // Store the directory of pages
    printf("Pager created successfully with data directory: %s\n", data_dir);
    return pager;
}

void free_pager(Pager* pager) {
    if (pager == NULL) return;
    free_LRUCache(pager->cache);
    free(pager);
    printf("Pager freed successfully.\n");
}

Page* pager_get(Pager *pager, int page_id) {
    if (pager == NULL || pager->cache == NULL) {
        printf("Pager or its cache is NULL!\n");
        return NULL;
    }

    // Try to get the page from the cache
    Page* page = LRUCache_get(pager->cache, page_id);
    if (page != NULL) {
        return page; // Cache hit
    }

    // Cache miss: Load the page from disk
    printf("Loading Page %d from disk.\n", page_id);
    page = load_page(page_id, pager->data_dir);
    if (page == NULL) {
        printf("Failed to load Page : %d!\n", page_id);
        return NULL;
    }

    // Put the newly created page into the cache
    if (LRUCache_put(pager->cache, page, pager->data_dir) != 0) {
        printf("Failed to put Page %d into cache!\n", page_id);
        free_page(page); // Free the page if it could not be added to cache; This is a memory leak prevention
        return NULL;
    }

    return page; // Return the newly loaded page
}