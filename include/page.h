#ifndef PAGE_H
#define PAGE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_NAME_SIZE 32 //Includes null character
#define MAX_EMAIL_SIZE 128 //Includes null character
#define PAGE_SIZE 4096 //4 KB (kilobyte)
#define NUM_ROWS_PAGE ((PAGE_SIZE - sizeof(size_t)) / (sizeof(Row) + sizeof(uint8_t)))

typedef struct {
    int64_t id;
    char name[MAX_NAME_SIZE];
    char email[MAX_EMAIL_SIZE];
} Row;

typedef struct {
    uint8_t row_exists[NUM_ROWS_PAGE];
    size_t num_rows;
} Header;

typedef struct {
    Header header;
    Row rows[NUM_ROWS_PAGE];
} Page;

Page* create_page();
void free_page(Page* page);
int page_find_row_id(Page* page, int64_t id); // returns -1 on failure, else returns the slot index in page
int page_find_row_name(Page* page, const char* name); // returns -1 on failure, else returns the slot index in page
int page_insert_row(Page* page, const Row* row); // returns 0 on success, 1 on failure
int page_delete_row(Page* page, size_t slot_index); // returns 0 on success, 1 on failure. Deletes row with given slot_index(not row id)

#endif //PAGE_H