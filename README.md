# BYoD : Group 2
---
## Part 1: In-Memory Storage
We have successfully implemented a `Table` structure to facilitate the storage of pages. Each of these pages is designed to hold data organized in the form of rows, allowing for efficient data management and retrieval.
### Data Model
Each record (row) consists of:
1. **ID** : stored in a 64 bit integer  
2. **Name** : stored in a string
3. **Age** : stored in a 32 bit integer
### Implemented Functions
- **Insert**: Add a new row.  
- **Delete**: Remove an existing record by row_id.  
- **Search**: Find and display a record by row_idr.  
- **Print All Data**: List every row in all the pages in the table.
---
### File Description
- **main.c**  
  It manages user interactions effectively and processes requests in order, ensuring that each interaction is handled accurately and efficiently.

- **table.c** 
 The heart of the code containing functions for row insertion, row deletion , scanning of rows based on row id and printing the complete database.

- **page.c** 
 Allocates memory for page creation and handles the freeing of the allocated memory as well. Also contains logic for row insertion in a particular page.

- **table.h**  
  It contains function declarations specifying the arguments for each function. It also contains the data structure definitions for the table:
  ```c
  typedef struct {
      Page* pages[TABLE_MAX_PAGES];
      size_t num_pages;
      size_t num_rows;
  } Table;
- **page.h**  
  Function declarations for page operations and the `Page` data structure definition:
  ```c
 
  typedef struct {
      int64_t id;
      int32_t age;
      char name[MAX_NAME_SIZE];
  } Row;

  typedef struct {
      Row    rows[NUM_ROWS_PAGE];
      bool   row_exists[NUM_ROWS_PAGE];
      size_t num_rows;  
  } Page;
  
  
### Build Instructions

1. Download the repository to your local system.
2. Launch the terminal in the directory where these files are located.
3. Type `make` in the terminal. This command will compile all the source files as outlined in the Makefile and you will be able to access all the above specified operations.
