#ifndef TABLE_H
#define TABLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "page.h"

#define TABLE_MAX_PAGES 100

typedef struct {
    Page* pages[TABLE_MAX_PAGES];
    size_t num_pages;
    size_t num_rows;
} Table;

typedef struct { // Pair to store a row's position
    int32_t page_slot; // Index of page in table
    int32_t row_slot;  // Index of row in page
} RowLoc;

// Note that this API provides no direct access to page insertion, deletion
// As pages are just internal implementation to deal with Rows 

Table* create_table();
void free_table(Table* table);
int table_find_id(Table* table, int64_t id, RowLoc* pos); // Updates RowLoc object, 1 if not found, 0 if found 
int table_find_name(Table* table, const char* name, RowLoc* pos); // Updates RowLoc object, 1 if not found, 0 if found
int table_insert(Table* table, const Row* row) // Inserts row, in the first empty page; const Row* as Row can be shallow copied
int table_insert_record(Table* table, int64_t id, int32_t age, const char* name, const char* email); // Requires updation if struct Row is updated
int table_delete_id(Table* table, int64_t id);
int table_delete_name(Table* table, const char* name);
void table_print(Table* table); // Prints whole table

#endif //TABLE_H
