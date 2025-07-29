#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include "table.h"
#include "util.h"


void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    Table* table = create_table();

    if (!table) {
        printf("Failed to create table!\n");
        return 1;
    }

    int service;
    int64_t id;
    char name[MAX_NAME_SIZE];
    char email[MAX_EMAIL_SIZE];
    RowLoc pos;

    print_magenta("WELCOME TO THE GROUP 2 DATABASE\n");

    while (true) {
        print_yellow("Choose an option:\n");
        print_cyan("1. Insert Record\n");
        print_cyan("2. Find Record by ID\n");
        print_cyan("3. Find Record by Name\n");
        print_cyan("4. Update Record by ID\n");
        print_cyan("5. Update Record by Name\n");
        print_cyan("6. Delete Record by ID\n");
        print_cyan("7. Delete Record by Name\n");
        print_cyan("8. Print All Records\n");
        print_cyan("9. Exit\n");
        print_cyan("10.Delete database files and exit\n");
        print_yellow("Enter your choice: ");
        
        if (scanf("%d", &service) != 1) {
            print_red("Invalid input. Please enter a number.\n");
            clear_input_buffer(); // Clear the invalid input
            continue;
        }
        clear_input_buffer();

        switch (service) {
            case 1:
                print_yellow("Enter ID: ");
                scanf("%" SCNd64, &id);
                clear_input_buffer();

                print_yellow("Enter Name: ");
                fgets(name, MAX_NAME_SIZE, stdin);
                name[strcspn(name, "\n")] = '\0'; // Remove trailing newline

                print_yellow("Enter Email: ");
                fgets(email, MAX_EMAIL_SIZE, stdin);
                email[strcspn(email, "\n")] = '\0';

                if (table_insert_record(table, id, name, email) == 0) {
                    print_green("Record inserted successfully!\n");
                } else {
                    print_red("Failed to insert record!\n");
                }
                break;
            case 2:
                print_yellow("Enter ID to find: ");
                scanf("%" SCNd64, &id);
                clear_input_buffer();

                if (table_find_id(table, id, &pos) == 0) {
                    printf("Record found at Page: %d, Row: %d\n", pos.page_slot, pos.row_slot);
                    Page* page = table_get_page(table, pos.page_slot);
                    Row* row = &page->rows[pos.row_slot]; 
                    printf("ID: %" PRId64 ", Name: %s, Email: %s\n", row->id, row->name, row->email);
                } else {
                    print_red("Failed to find record!\n");
                }
                break;
            case 3:
                print_yellow("Enter Name to find: ");
                fgets(name, MAX_NAME_SIZE, stdin);
                name[strcspn(name, "\n")] = '\0';

                if (table_find_name(table, name, &pos) == 0) {
                    printf("Record found at Page: %d, Row: %d\n", pos.page_slot, pos.row_slot);
                    Page* page = table_get_page(table, pos.page_slot);
                    Row* row = &page->rows[pos.row_slot]; 
                    printf("ID: %" PRId64 ", Name: %s, Email: %s\n", row->id, row->name, row->email);
                } else {
                    print_red("Failed to find record!\n");
                }
                break;
            case 4:
                print_yellow("Enter ID to update: ");
                scanf("%" SCNd64, &id);
                clear_input_buffer();

                if (table_find_id(table, id, &pos) == 0) {
                    printf("Record found at Page: %d, Row: %d\n", pos.page_slot, pos.row_slot);
                    Page* page = table_get_page(table, pos.page_slot);
                    Row* row = &page->rows[pos.row_slot]; 

                    print_yellow("Enter Name: ");
                    fgets(row->name, MAX_NAME_SIZE, stdin);
                    row->name[strcspn(row->name, "\n")] = '\0'; // Remove trailing newline

                    print_yellow("Enter Email: ");
                    fgets(row->email, MAX_EMAIL_SIZE, stdin);
                    row->email[strcspn(row->email, "\n")] = '\0';

                } else {
                    print_red("Failed to update record!\n");
                }
                break;
            case 5:
                print_yellow("Enter Name to update: ");
                fgets(name, MAX_NAME_SIZE, stdin);
                name[strcspn(name, "\n")] = '\0';

                if (table_find_name(table, name, &pos) == 0) {
                    printf("Record found at Page: %d, Row: %d\n", pos.page_slot, pos.row_slot);
                    Page* page = table_get_page(table, pos.page_slot);
                    Row* row = &page->rows[pos.row_slot]; 

                    print_yellow("Enter Name: ");
                    fgets(row->name, MAX_NAME_SIZE, stdin);
                    row->name[strcspn(row->name, "\n")] = '\0'; // Remove trailing newline

                    print_yellow("Enter Email: ");
                    fgets(row->email, MAX_EMAIL_SIZE, stdin);
                    row->email[strcspn(row->email, "\n")] = '\0';

                } else {
                    print_red("Failed to update record!\n");
                }
                break;
            case 6:
                print_yellow("Enter ID to delete: ");
                scanf("%" SCNd64, &id);
                clear_input_buffer();

                if (table_delete_id(table, id) == 0) {
                    print_green("Record deleted successfully!\n");
                } else {
                    print_red("Failed to delete record!\n");
                }
                break;
            case 7:
                print_yellow("Enter Name to delete: ");
                fgets(name, MAX_NAME_SIZE, stdin);
                name[strcspn(name, "\n")] = '\0';

                if (table_delete_name(table, name) == 0) {
                    print_green("Record deleted successfully!\n");
                } else {
                    print_red("Failed to delete record!\n");
                }
                break;
            case 8:
                print_magenta("GROUP 2 DATABASE:\n");
                table_print(table);
                break;
            case 9:
                print_magenta("Thank you for using Group 2 Database!\n");
                free_table(table); 
                return 0;
            case 10:
                print_magenta("Thank you for using Group 2 Database!\n");
                print_yellow("Deleting database files...\n");
                // Clear the pager's data directory
                if (table->pager && table->pager->data_dir) {
                    char command[256];
                    snprintf(command, sizeof(command), "rm -rf %s/*", table->pager->data_dir);
                    system(command); // Use system call to delete files in the data directory
                }
                print_magenta("Database files cleared successfully!\n");
                free_table(table); 
                return 0;
            default:
                print_red("Invalid choice! Please try again.\n");
                break;
        }
    }
}