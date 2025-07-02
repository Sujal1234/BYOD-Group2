#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
    
    for (int i = 0; i < table->num_pages; i++) {
        if (table->pages[i] && table->pages[i]->num_rows < ROWS_PER_PAGE) {
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
    int slot = -1;
    for (int i = 0; i < ROWS_PER_PAGE; i++) {
        if (!target_page->row_exists[i]) {
            slot = i;
            break;
        }
    }

    if(slot == -1){
        printf("Error: Number of rows in page is less than max but all rows are occupied\n");
        return 1;
    }

    target_page->rows[slot] = *row;
    target_page->row_exists[slot] = true;
    target_page->num_rows++;
    table->num_rows++;
}

int table_insert_record(Table* table, int64_t id, int32_t age, const char* name){
    if(strlen(name)+1 > MAX_NAME_SIZE){
        int return_flag = 0;
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