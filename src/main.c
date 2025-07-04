#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include "page.h"
#include "table.h"


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

    printf("WELCOME TO THE GROUP 2 DATABASE\n");
    while(true) {
        printf("Choose an option:\n");
        printf("1. Insert Record\n");
        printf("2. Delete Record by ID\n");
        printf("3. Delete Record by Name\n");
        printf("4. Print All Records\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &service);
        switch(service) {
            case 1:
                printf("Enter ID: ");
                scanf("%" SCNd64, &id);
                printf("Enter Age: ");
                scanf("%" SCNd32, &age);
                printf("Enter Name: ");
                scanf("%s", name);
                
                if(table_insert_record(table, id, age, name) == 0) {
                    printf("Record inserted successfully!\n");
                } else {
                    printf("Failed to insert record!\n");
                }
                break;  
            case 2:
                printf("Enter ID to delete: ");
                scanf("%" SCNd64, &id);
                
                if(delete_row_id(table, id)) {
                    printf("Record deleted successfully!\n");
                } else {
                    printf("Failed to delete record!\n");
                }
                break;
                
            case 3:
                printf("Enter Name to delete: ");
                scanf("%s", name);
                
                if(delete_row_name(table, name)) {
                    printf("Record deleted successfully!\n");
                } else {
                    printf("Failed to delete record!\n");
                }
                break;
                
            case 4:
                printf("GROUP 2 DATABASE:\n");
                print_table(table);
                break;
                
            case 5:
                printf("Thank you for using Group 2 Database!\n");
                return 0;
                
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    }
}
