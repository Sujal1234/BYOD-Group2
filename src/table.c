#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "table.h"
#include "page.h"

static void table_insert_row(Table* table, Row* row);

void table_insert_page(Table* table){
    if(table->num_pages >= TABLE_MAX_PAGES){
        printf("Table is full, cannot insert more pages\n");
        return;
    }
    Page* new_page = (Page*) calloc(1, sizeof(Page));
    if(new_page == NULL){
        perror("memory allocation of new page in table");
        return;
    }
    table->pages[table->num_pages] = new_page;

    table->num_pages++;
}

static void table_insert_row(Table* table, Row* row){
    if(table->num_pages == 0){
        table_insert_page(table);
    }

    Page* page = table->pages[table->num_pages-1];
    if(page->num_rows == NUM_ROWS_PAGE){
        //Page is full
        if(table->num_pages == TABLE_MAX_PAGES){
            //Table cannot store any more pages
            printf("Insufficient space in table\n");
            return;
        }
        assert(table->num_pages < TABLE_MAX_PAGES);

        //Make a new page since last page is full
        table_insert_page(table);
        page_insert_row(table->pages[table->num_pages-1], row);
        table->num_rows++;
        return;
    }

    page_insert_row(page, row);
    table->num_rows++;
}

void table_insert_record(Table* table, int64_t id, int32_t age, char* name){
    if(strlen(name)+1 > MAX_NAME_SIZE){
        printf("Name too long\n");
        return;
    }
    if(age <= 0){
        printf("Age cannot be non-positive\n");
        return;
    }
    if(id < 0){
        printf("ID cannot be negative\n");
        return;
    }

    Row row = {
        .id = id,
        .age = age,
        .name = ""
    };
    strcpy(row.name, name);

    table_insert_row(table, &row);
}