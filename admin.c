//
//  main.c
//  admin
//
//  Created by Kyle Horne on 2019-04-01.
//  Copyright © 2019 Kyle Horne. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/msg.h>

#include "common.h"

// Get input from standard in
void get_input(char* str) {
    printf("%s: ", str);
    char buffer[BUFSIZ];
    fgets(buffer, BUFSIZ, stdin);
    strcpy(msg.payload, buffer);
}

int main(int argc, const char * argv[]) {
    // Instantiate the messaeg queue
    int to_bookkeeper = create_bookkeeper_msg_queue();
    int to_admin = create_admin_msg_queue();
    // Listen for request from standard in
    int running = 1;
    while(running) {
        // Parse the procedure call
        enum ACTION action = get_procedure();
        if (action == INSERT) {
            // Insert handler
            strcpy(msg.payload, PROCEDURE[INSERT]);
            send_msg(to_bookkeeper);
            get_input("Enter the employee name");
            send_msg(to_bookkeeper);
            get_input("Enter the department name");
            send_msg(to_bookkeeper);
            get_input("Enter the employee id");
            send_msg(to_bookkeeper);
            get_input("Enter the employee salary");
            send_msg(to_bookkeeper);
        } else if (action == CHECK_NAME) {
            // Check name handler
            strcpy(msg.payload, PROCEDURE[CHECK_NAME]);
            send_msg(to_bookkeeper);
            get_input("Enter the employee id");
            send_msg(to_bookkeeper);
            char* res = rcv_msg(to_admin);
            if (strcmp(res, "err") == 0) {
                printf("Employee does not exist.\n");
            } else {
                printf("Employee name: %s", res);
            }
        } else if (action == CHECK_DEPARTMENT) {
            // Check department handler
            strcpy(msg.payload, PROCEDURE[CHECK_DEPARTMENT]);
            send_msg(to_bookkeeper);
            get_input("Enter the employee id");
            send_msg(to_bookkeeper);
            char* res = rcv_msg(to_admin);
            if (strcmp(res, "err") == 0) {
                printf("Employee does not exist.\n");
            } else {
                printf("Department name: %s", res);
            }
        } else if (action == CHECK_SALARY) {
            // Check salary handler
            strcpy(msg.payload, PROCEDURE[CHECK_SALARY]);
            send_msg(to_bookkeeper);
            get_input("Enter the employee id");
            send_msg(to_bookkeeper);
            char* res = rcv_msg(to_admin);
            if (strcmp(res, "err") == 0) {
                printf("Employee does not exist.\n");
            } else {
                printf("Employee salary: %s\n", res);
            }
        } else if (action == CHECK_EMPLOYEE_NUMBER) {
            // Check employee number handler
            strcpy(msg.payload, PROCEDURE[CHECK_EMPLOYEE_NUMBER]);
            send_msg(to_bookkeeper);
            get_input("Enter the employee name");
            send_msg(to_bookkeeper);
            char* res = rcv_msg(to_admin);
            if (strcmp(res, "err") == 0) {
                printf("Employee does not exist.\n");
            } else {
                printf("Employee id: %s\n", res);
            }
        }  else if (action == CHECK) {
            // Check handler
            strcpy(msg.payload, PROCEDURE[CHECK]);
            send_msg(to_bookkeeper);
            get_input("Enter the department name");
            send_msg(to_bookkeeper);
            // Listen to respones until stopping condition is met
            int is_eof = 1;
            int counter = 0;
            while(is_eof) {
                char* id = rcv_msg(to_admin);
                if (strcmp(id, "eof") == 0) {
                    is_eof = 0;
                } else {
                    counter++;
                    printf("Employee id: %s\n", id);
                }
            }
            if (counter == 0) {
                printf("There are no employees in this department.\n");
            }
        } else if (action == DELETE) {
            // Delete handler
            strcpy(msg.payload, PROCEDURE[DELETE]);
            send_msg(to_bookkeeper);
            get_input("Enter the employee id");
            send_msg(to_bookkeeper);
            printf("Delete result: %s\n", rcv_msg(to_admin));
        } else if (action == EXIT) {
            // Exit handler
            strcpy(msg.payload, PROCEDURE[EXIT]);
            send_msg(to_bookkeeper);
            running = 0;
            // Exit loop
        } else if (action == ERROR) {
            // Invalid request 
            printf("Invalid procedure: options are: <insert, check_department, check_salary, check_employee_number, check, delete, exit>\n");
        }
    }
    exit(EXIT_SUCCESS);
}
