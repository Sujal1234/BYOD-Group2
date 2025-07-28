#ifndef PAGER_H
#define PAGER_H

#define CACHE_SIZE 10 // Maximum number of pages in the cache
#include "page.h"

typedef struct  LRUCache;
// LRUCache is meant to be used by pager internally, so no need to access it directly from outside

typedef struct {
    LRUCache* cache;
    const char* data_dir; // Directory where the pages are stored
} Pager;

Pager* create_pager(const char* data_dir);
void free_pager(Pager* pager);
Page* pager_get(Pager *pager, int page_id);
// int pager_flush(Pager *pager, Page *page); // we never actually explicitly delete a page, so this is not needed; This is used internally before removing from LRU


#endif //PAGER_H