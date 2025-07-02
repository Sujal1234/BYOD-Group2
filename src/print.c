#include <stdio.h>
#include <string.h>
#include "page.h"
#include "table.h"

void print_table(Table* table){
    if(table -> num_pages==0){
        printf("Table is empty. No data to show\n");
        return;
    }

    
    for(size_t j = 0; j<table->num_pages; j++){

        Page* page = table->pages[j];
        if(page!=NULL){
            printf("Page no: %d\n", j);

            for(size_t k = 0; k<page->num_rows; k++){
                Row* row = &page->rows[k];
                if(row!=NULL)
                    printf("S.No: %d, ID: %lld, AGE: %d, NAME = %s\n",k,row->id,row->age,row->name);
                
            }

            printf("\n");
        }
    }
}