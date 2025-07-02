#include <stdio.h>
#include <stdlib.h>

#include "page.h"

Page* create_page(){
    Page* page = calloc(1, sizeof(Page));
    return page;
}

void free_page(Page* page){
    if(page){
        free(page);
    }    
}

int page_insert_row(Page* page, const Row* row){
    if(page->num_rows == NUM_ROWS_PAGE){
        printf("Insufficient space in page\n");
        return 1;
    }
    page->rows[page->num_rows] = *row;
    page->row_exists[page->num_rows] = true;
    page->num_rows++;
    return 0;
}