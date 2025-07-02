#include<stdio.h>
#include "table.h"
#include "page.h"

// Function to check for empty table.
void scan(Table* table, int64_t id){
    if(table->num_pages == 0){
        printf("Table is empty. No rows to scan\n");
        return;
    }

// Simple loop which scans all pages and all rows in those pages to look for valid rows
    int flag = 0;
    for(size_t i = 0; i<table->num_pages; i++){
        Page* page = table->pages[i];
        for(size_t j = 0; j<page->num_rows; j++){
            Row* row = &page->rows[j];
            if(row->id == id){
                printf("Row found with id = %lld, age = %d, name = \"%s\"\n",(long long)id,row->age,row->name);
                flag = 1;
            }
        }
    }
    if(!flag){
        printf("No row with id = %lld exists.\n",(long long)id);
    }

}
