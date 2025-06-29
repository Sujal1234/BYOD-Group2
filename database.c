#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_NAME_SIZE 64 //Includes null character
#define PAGE_SIZE 4096 //4 KB (kibibyte)
#define MAX_PAGES 100

typedef struct {
    int64_t id;
    int32_t age;
    char name[MAX_NAME_SIZE];
} Row;

#define NUM_ROWS_PAGE (PAGE_SIZE/sizeof(Row))

typedef struct {
    size_t num_rows;
    char data[PAGE_SIZE];
    size_t free_space;
    size_t used_space;
} Page;

const size_t rows_per_page = (size_t) PAGE_SIZE / sizeof(Row);
const size_t table_max_rows = (size_t) MAX_PAGES * rows_per_page;

typedef struct {
    Page* pages[MAX_PAGES];
    size_t num_pages;
    size_t num_rows;
} Table;

int main(){
    return 0;
}