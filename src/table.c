#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>

#include "table.h"
#include "page.h"

static int table_insert_page(Table* table); // Inserts empty page

Table* create_table(){
    Table* table = calloc(1, sizeof(Table));
    return table;
}

void free_table(Table* table){
    if(!table) return;
    for (size_t i = 0; i < table->num_pages; i++)
    {
        free_page(table->pages[i]);
    }
    free(table);
}

static int table_insert_page(Table* table){
    if(table->num_pages >= TABLE_MAX_PAGES){
        printf("Table is full, cannot insert more pages\n");
        return 1;
    }
    Page* new_page = (Page*) calloc(1, sizeof(Page));
    if(new_page == NULL){
        perror("Memory allocation of new page in table failed");
        return 1;
    }
    table->pages[table->num_pages] = new_page;
    table->num_pages++;
    return 0;
}

// Doesn't print anything, just updates the RowLoc object
int table_find_id(Table* table, int64_t id, RowLoc* pos){
    if(table->num_pages == 0){
        printf("Table is empty. No rows to scan\n");
        return 1;
    }

    // Simple loop which scans all pages and all rows in those pages to look for valid rows
    for(size_t i = 0; i < table->num_pages; i++){
        Page* page = table->pages[i];
        int ind = page_find_row_id(page, id);
        if(ind != -1){
            pos->page_slot = i;
            pos->row_slot = ind;
            return 0;
        }
    }
    pos->page_slot = -1;
    pos->row_slot = -1;
    return 1;
}

// Doesn't print anything, just updates the RowLoc object
int table_find_name(Table* table, const char* name, RowLoc* pos){
    if(table->num_pages == 0){
        printf("Table is empty. No rows to scan\n");
        return 1;
    }
    // Simple loop which scans all pages and all rows in those pages to look for valid rows
    for(size_t i = 0; i < table->num_pages; i++){
        Page* page = table->pages[i];
        int ind = page_find_row_name(page, name);
        if(ind != -1){
            pos->page_slot = i;
            pos->row_slot = ind;
            return 0;
        }
    }
    pos->page_slot = -1;
    pos->row_slot = -1;
    return 1;
}

int table_insert(Table* table, const Row* row){ // do check if row id is already present or not
    if(!table || !row){
        return 1;
    }
    if(table->num_pages == 0){
        if(table_insert_page(table)){
            return 1;
        }
    }
    RowLoc pos;
    if(!table_find_id(table, row->id, &pos)){
        printf("Row with this id already exists.\n");
        return 1;
    }
    //Find a page with free space
    Page* target_page = NULL;
    for (size_t i = 0; i < table->num_pages; i++) {
        if (table->pages[i] && table->pages[i]->header.num_rows < NUM_ROWS_PAGE) {
            target_page = table->pages[i];
            break;
        }
    }
    //If no page with free space found, create a new page
    if(!target_page){
        if(table_insert_page(table)){
            return 1;
        }
        target_page = table->pages[table->num_pages - 1];
    }
    table->num_rows++;
    return page_insert_row(target_page, row);
}

int table_insert_record(Table* table, int64_t id, int32_t age, const char* name, const char* email){
    int return_flag=0;
    if(strlen(name)+1 > MAX_NAME_SIZE){
        printf("Name too long\n");
        return_flag = 1;
    }
    if(age <= 0){
        printf("Age cannot be non-positive\n");
        return_flag = 1;
    }
    if(id < 0){
        printf("ID cannot be negative\n");
        return_flag = 1;
    }
    if(return_flag){
        return 1;
    }
    Row row = {
        .id = id,
        .age = age,
        .name = "",
        .email = ""
    };
    strcpy(row.name, name);
    strcpy(row.email, email);
    return table_insert(table, &row);
}

// Returns 0 if row is successfully deleted, 1 otherwise.
int table_delete_id(Table* table, int64_t id){
    if(!table){
        printf("Table is empty!\n");
        return 1;
    }
    if(table->num_pages == 0){
        printf("Table is empty!\n");
        return 1;
    }
    for(size_t i = 0; i < table->num_pages; i++){ 
        Page* curr_page = table->pages[i];
        int ind = page_find_row_id(curr_page, id);
        if(ind != -1) {
            return page_delete_row(curr_page, ind);
        }
    }
    printf("No row has been found with the specified ID!\n");
    return 1;
}

// Returns 0 if row is successfully deleted, 1 otherwise.
int table_delete_name(Table* table, const char* name){
    if(!table){
        printf("Table is empty!\n");
        return 1;
    }
    if(table->num_pages == 0){
        printf("Table is empty!\n");
        return 1;
    }
    for(size_t i = 0; i < table->num_pages; i++){ 
        Page* curr_page = table->pages[i];
        int ind = page_find_row_name(curr_page, name);
        if(ind != -1) {
            return page_delete_row(curr_page, ind);
        }
    }
    printf("No row has been found with the specified name!\n");
    return 1;
}

void table_print(Table* table){
    if(!table){
        printf("Table is NULL\n");
        return;
    }
    if(table->num_pages == 0){
        printf("Table is empty. No data to show\n");
        return;
    }
    for(size_t i = 0; i < table->num_pages; i++){
        Page* page = table->pages[i];
        if(page == NULL){
            break;
        }
        printf("Page no: %zu\n", i);
        size_t j = 0, rows_printed = 0;
        while(rows_printed < page->header.num_rows){ 
            if(!page->header.row_exists[j]){
                j++;
                continue;  // Skip deleted rows
            }
            Row* row = &page->rows[j];
            assert(row != NULL);  // Ensure row is not NULL
            printf("S.No: %zu, ID: %" PRId64 ", AGE: %" PRId32 ", NAME = %s, EMAIL = %s\n",
                rows_printed, row->id, row->age, row->name, row->email);
            j++;
            rows_printed++;
        }
        printf("\n");
    }
}