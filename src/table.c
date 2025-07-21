#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include<stdbool.h>
#include <inttypes.h>

#include "table.h"
#include "page.h"

static int table_insert_row(Table* table, const Row* row);

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

int table_insert_page(Table* table){
    if(table->num_pages >= TABLE_MAX_PAGES){
        printf("Table is full, cannot insert more pages\n");
        return 1;
    }
    Page* new_page = (Page*) calloc(1, sizeof(Page));
    if(new_page == NULL){
        perror("memory allocation of new page in table");
        return 1;
    }
    table->pages[table->num_pages] = new_page;

    table->num_pages++;
    return 0;
}

static int table_insert_row(Table* table, const Row* row){
    if(!table || !row){
        return 1;
    }

    if(table->num_pages == 0){
        if(table_insert_page(table)){
            return 1;
        }
    }

    //Find a page with free space
    Page* target_page = NULL;
    
    for (size_t i = 0; i < table->num_pages; i++) {
        if (table->pages[i] && table->pages[i]->num_rows < NUM_ROWS_PAGE) {
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

    //Find first available row in the target page
    size_t slot;
    bool foundSlot = false;
    for (size_t i = 0; i < NUM_ROWS_PAGE; i++) {
        if (!target_page->row_exists[i]) {
            slot = i;
            foundSlot = true;
            break;
        }
    }

    if(!foundSlot){
        printf("Error: Number of rows in page is less than max but all rows are occupied\n");
        return 1;
    }

    target_page->rows[slot] = *row;
    target_page->row_exists[slot] = true;
    target_page->num_rows++;
    table->num_rows++;
    return 0;
}

int table_insert_record(Table* table, int64_t id, int32_t age, const char* name){
    int return_flag=0;
    if(strlen(name)+1 > MAX_NAME_SIZE){
        return_flag = 0;
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
        .name = ""
    };
    strcpy(row.name, name);

    if(table_insert_row(table, &row)){
        return 1;
    }
    return 0;
}

void print_table(Table* table){
    if(!table){
        printf("Table is NULL\n");
        return;
    }

    if(table -> num_pages==0){  //if table is empty
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
        while(rows_printed < page->num_rows){ 
            if(!page->row_exists[j]){
                j++;
                continue;  // Skip deleted rows
            }

            Row* row = &page->rows[j];
            assert(row != NULL);  // Ensure row is not NULL

            printf("S.No: %zu, ID: %" PRId64 ", AGE: %" PRId32 ", NAME = %s\n",
                rows_printed, row->id, row->age, row->name);
            j++;
            rows_printed++;
        }
        printf("\n");
    }
}

// Function to find a row.
// Returns 0 if row is found, 1 otherwise.
int scan(Table* table, int64_t id){
    if(table->num_pages == 0){
        printf("Table is empty. No rows to scan\n");
        return 1;
    }

    // Simple loop which scans all pages and all rows in those pages to look for valid rows
    for(size_t i = 0; i < table->num_pages; i++){
        Page* page = table->pages[i];
        for(size_t j = 0; j < page->num_rows; j++){
            if(!page->row_exists[j]){
                continue;  //skip deleted rows
            }
            Row* row = &page->rows[j];
            if(row->id == id){
                printf("Row found with id = %lld, age = %d, name = \"%s\"\n",(long long)id,row->age,row->name);
                return 0;
            }
        }
    }
    printf("No row with id = %lld exists.\n",(long long)id);
    return 1;
}

// Returns 0 if row is successfully deleted, 1 otherwise.
int delete_row_id(Table* table, int64_t id){
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

        for(size_t j = 0; j < NUM_ROWS_PAGE; j++){
            if (curr_page->row_exists[j] && curr_page->rows[j].id == id) {
                    // Clear row record
                    curr_page->row_exists[j] = false;
                    curr_page->rows[j].id = 0;
                    curr_page->rows[j].age = 0;
                    curr_page->rows[j].name[0] = '\0';
                    curr_page->num_rows--;
                    table->num_rows--;
                    printf("Row deleted!\n");
                    return 0;
            } 
        }
    }
    printf("No row has been found with the specified ID!\n");
    return 1;
}

// Returns 0 if row is successfully deleted, 1 otherwise.
int delete_row_name(Table* table, const char* name){
    if(!table){
        printf("Table is empty!\n");
        return 1;
    }
    if(table->num_pages==0){
        printf("Table is empty!\n");
        return 1;
    }
    for(size_t i = 0; i < table->num_pages; i++){
        Page* curr_page = table->pages[i];

        for(size_t j = 0; j < NUM_ROWS_PAGE; j++){
            if (curr_page->row_exists[j] && strcmp(curr_page->rows[j].name, name) == 0) {
                    curr_page->row_exists[j] = false;
                    curr_page->rows[j].id = 0;
                    curr_page->rows[j].age = 0;
                    curr_page->rows[j].name[0] = '\0';                    
                    curr_page->num_rows--;
                    table->num_rows--;
                    printf("Row deleted!\n");
                    return 0;
            } 
        }
    }
    printf("No row has been found with the specified name!\n");
    return 1;
}
