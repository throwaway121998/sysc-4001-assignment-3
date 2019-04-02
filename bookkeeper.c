//
//  main.c
//  bookkeeper
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

typedef char Name[MAX_TEXT];
typedef struct {
    Name name;
    Name department;
    int  id;
    int  salary;
} Employee;

struct Node {
    Employee employee;
    struct Node* next;
    struct Node* prev;
};

// Push a new employee to the list
void push(struct Node** head_ref, Employee employee) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->employee = employee;
    // Point new node to head
    new_node->next = (*head_ref);
    new_node->prev = NULL;
    // Set head prev to new node
    if ((*head_ref) != NULL) {
        (*head_ref)->prev = new_node;
    }
    // Point head to new node
    (*head_ref) = new_node;
}

// Delete an employee from the list
void del(struct Node** head_ref, struct Node* del) {
    // List is empty
    if (*head_ref == NULL || del == NULL) {
        return;
    }
    // If deleted node is the head node
    if (*head_ref == del) {
        *head_ref = del->next;
    }
    // Change next if the deleted node is not last element
    if (del->next != NULL) {
        del->next->prev = del->prev;
    }
    // Change prev if the deleted node is not the first element
    if (del->prev != NULL) {
        del->prev->next = del->next;
    }
    // Free the delete node
    free(del);
}

// Print the database of employees
void printDB(struct Node* node) {
    while (node != NULL) {
        printf("Employee name: %s", node->employee.name);
        printf("Department name: %s", node->employee.department);
        printf("Employee id: %d\n", node->employee.id);
        printf("Employee salary: %d\n", node->employee.salary);
        node = node->next;
    }
}

// Delete message queue
void del_msg_queue(int msgid) {
    if (msgctl(msgid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, const char * argv[]) {
    // Instantiate the message queue
    int to_bookkeeper = create_bookkeeper_msg_queue();
    int to_admin = create_admin_msg_queue();
    // Reference to the list of employees
    struct Node* head = NULL;
    // Listen for RPCs...
    int running = 1;
    while(running) {
        // Parse the procedure call
        char* buffer = rcv_msg(to_bookkeeper);
        if (strncmp(buffer, PROCEDURE[INSERT], 6) == 0) {
            // Insert handler
            Employee employee;
            // Instantiate a new employee
            strcpy(employee.name, rcv_msg(to_bookkeeper));
            strcpy(employee.department, rcv_msg(to_bookkeeper));
            employee.id = atoi(rcv_msg(to_bookkeeper));
            employee.salary = atoi(rcv_msg(to_bookkeeper));
            // Push the item on the employee list
            push(&head, employee);
            // Print the employee list
            printDB(head);
        } else if (strncmp(buffer, PROCEDURE[CHECK_NAME], 10) == 0) {
            // Check name handler
            int id = atoi(rcv_msg(to_bookkeeper));
            struct Node* node = head;
            while(node != NULL) {
                // Find the employee id that matches
                if (node->employee.id == id) {
                    // Send the name of the employee back to the client
                    strcpy(msg.payload, node->employee.name);
                    send_msg(to_admin);
                    break;
                }
                node = node->next;
            }
        } else if (strncmp(buffer, PROCEDURE[CHECK_DEPARTMENT], 16) == 0) {
            // Check department handler
            int id = atoi(rcv_msg(to_bookkeeper));
            struct Node* node = head;
            while(node != NULL) {
                // Find the employee id that matches
                if (node->employee.id == id) {
                    // Send the name of the department back to the client
                    strcpy(msg.payload, node->employee.department);
                    send_msg(to_admin);
                    break;
                }
                node = node->next;
            }
        } else if (strncmp(buffer, PROCEDURE[CHECK_SALARY], 12) == 0) {
            // Check salary handler
            int id = atoi(rcv_msg(to_bookkeeper));
            struct Node* node = head;
            while(node != NULL) {
                // Find the employee id that matches
                if (node->employee.id == id) {
                    // Send the salary of the employee back to the client
                    char buffer[MAX_TEXT];
                    sprintf(buffer, "%d", node->employee.salary);
                    strcpy(msg.payload, buffer);
                    send_msg(to_admin);
                    break;
                }
                node = node->next;
            }
        } else if (strncmp(buffer, PROCEDURE[CHECK_EMPLOYEE_NUMBER], 21) == 0) {
            // Check employee number handler
            char* name = rcv_msg(to_bookkeeper);
            struct Node* node = head;
            while(node != NULL) {
                // Find the employee name that matches
                if (strcmp(node->employee.name, name) == 0) {
                    // Send the id of the employee back to the client
                    char buffer[MAX_TEXT];
                    sprintf(buffer, "%d", node->employee.id);
                    strcpy(msg.payload, buffer);
                    send_msg(to_admin);
                    break;
                }
                node = node->next;
            }
        } else if (strncmp(buffer, PROCEDURE[CHECK], 5) == 0) {
            // Check handler
            char department[MAX_TEXT];
            strcpy(department, rcv_msg(to_bookkeeper));
            struct Node* node = head;
            while(node != NULL) {
                // Find the employee department that matches
                if (strcmp(node->employee.department, department) == 0) {
                    // Send the id of the employee back to the client
                    char buffer[MAX_TEXT];
                    sprintf(buffer, "%d", node->employee.id);
                    strcpy(msg.payload, buffer);
                    send_msg(to_admin);
                    // Loop again
                }
                node = node->next;
            }
            // Notify client that there are no more eployees to check
            strcpy(msg.payload, "eof");
            send_msg(to_admin);
        } else if (strncmp(buffer, PROCEDURE[DELETE], 6) == 0) {
            // Delete handler
            int id = atoi(rcv_msg(to_bookkeeper));
            struct Node* node = head;
            int has_found = -1;
            while(node != NULL) {
                // Find the employee id that matches
                if (node->employee.id == id) {
                    has_found = 0;
                    break;
                }
                node = node->next;
            }
            // Delete the employee
            if (has_found == 0) {
                del(&head, node);
            }
            // Print the employee list
            printDB(head);
            // Notify client if successful
            char buffer[MAX_TEXT];
            sprintf(buffer, "%d", has_found);
            strcpy(msg.payload, buffer);
            send_msg(to_admin);
        } else if (strncmp(buffer, PROCEDURE[EXIT], 4) == 0) {
            // Exit handler
            running = 0;
            // Exit loop
        }
    }
    // Delete all message queues
    del_msg_queue(to_bookkeeper);
    del_msg_queue(to_admin);
    // Exit
    exit(EXIT_SUCCESS);
}

