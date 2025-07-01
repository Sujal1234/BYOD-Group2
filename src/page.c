#include <stdio.h>

#include "page.h"

void page_insert_row(Page* page, Row* row){
    if(page->num_rows == NUM_ROWS_PAGE){
        printf("Insufficient space in page\n");
        return;
    }
    page->rows[page->num_rows] = *row;
    page->num_rows++;
}