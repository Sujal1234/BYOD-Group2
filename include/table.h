#ifndef TABLE_H
#define TABLE_H

#include <stddef.h>
#include <stdint.h>

#include "page.h"

#define TABLE_MAX_PAGES 100

typedef struct {
    Page* pages[TABLE_MAX_PAGES];
    size_t num_pages;
    size_t num_rows;
} Table;

Table* create_table();
void free_table(Table* table);
int table_insert_page(Table* table);
int table_insert_record(Table* table, int64_t id, int32_t age, const char* name);
void print_table(Table* table);
void scan(Table* table, int64_t id);

#endif //TABLE_H
