#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "page.h"

// Note that the row find loops run for NUM_ROWS_PAGE, as the page is fixed size

Page* create_page(){
    Page* page = calloc(1, sizeof(Page));
    return page;
}

void free_page(Page* page){
    if(page){
        free(page);
    }    
}

int page_find_row_id(Page* page, int64_t id) {
    for(size_t i = 0; i < NUM_ROWS_PAGE; i++){
        if(page->rows[i].id == id && page->header.row_exists[i]){
            return i;
        }
    }
    return -1;
}

int page_find_row_name(Page* page, const char* name) {
    for(size_t i = 0; i < NUM_ROWS_PAGE; i++){
        if(strcmp(page->rows[i].name, name) == 0 && page->header.row_exists[i]){
            return i;
        }
    }
    return -1;
}


int page_insert_row(Page* page, const Row* row){
    if(page->header.num_rows == NUM_ROWS_PAGE){
        printf("Insufficient space in page\n");
        return 1;
    }
    int empty_ind = 0;
    for(size_t i = 0; i < NUM_ROWS_PAGE; i++){
        if(!page->header.row_exists[i]){
            empty_ind = i;
            break;
        }
    }
    page->rows[empty_ind] = *row;
    page->header.row_exists[empty_ind] = 1;
    page->header.num_rows++;
    return 0;
}

int page_delete_row(Page* page, size_t slot_index) {
    if(slot_index >= page->header.num_rows || !page->header.row_exists[slot_index]){
        return 1;
    }
    page->header.row_exists[slot_index] = 0;
    page->header.num_rows--;
    return 0;
}