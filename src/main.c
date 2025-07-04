#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include "page.h"
#include "table.h"
#include "util.h"

int main(){
    Table* table = create_table();

    if(!table) {
        printf("Failed to create table!\n");
        return 1;
    }

    int service;
    int64_t id;
    int32_t age;
    char name[MAX_NAME_SIZE];

    // printf("WELCOME TO THE GROUP 2 DATABASE\n");
    print_magenta("WELCOME TO THE GROUP 2 DATABASE\n");

    while(true) {
        print_yellow("Choose an option:\n");
        print_cyan("1. Insert Record\n");
        print_cyan("2. Delete Record by ID\n");
        print_cyan("3. Delete Record by Name\n");
        print_cyan("4. Print All Records\n");
        print_cyan("5. Exit\n");
        print_yellow("Enter your choice: ");
        scanf("%d", &service);
        switch(service) {
            case 1:
                print_yellow("Enter ID: ");
                scanf("%" SCNd64, &id);
                print_yellow("Enter Age: ");
                scanf("%" SCNd32, &age);
                print_yellow("Enter Name: ");
                scanf("%s", name);
                
                if(table_insert_record(table, id, age, name) == 0) {
                    print_green("Record inserted successfully!\n");
                } else {
                    print_red("Failed to insert record!\n");
                }
                break;  
            case 2:
                print_yellow("Enter ID to delete: ");
                scanf("%" SCNd64, &id);
                
                if(delete_row_id(table, id) == 0) {
                    print_green("Record deleted successfully!\n");
                } else {
                    print_red("Failed to delete record!\n");
                }
                break;
                
            case 3:
                print_yellow("Enter Name to delete: ");
                scanf("%s", name);
                
                if(delete_row_name(table, name) == 0) {
                    print_green("Record deleted successfully!\n");
                } else {
                    print_red("Failed to delete record!\n");
                }
                break;
                
            case 4:
                print_magenta("GROUP 2 DATABASE:\n");
                print_table(table);
                break;
                
            case 5:
                print_magenta("Thank you for using Group 2 Database!\n");
                return 0;
                
            default:
                print_red("Invalid choice! Please try again.\n");
                break;
        }
    }
}
