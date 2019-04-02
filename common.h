//
//  common.h
//  admin
//
//  Created by Kyle Horne on 2019-04-01.
//  Copyright © 2019 Kyle Horne. All rights reserved.
//

#ifndef common_h
#define common_h

#define MAX_TEXT 32
typedef struct {
    long int type;
    char payload[MAX_TEXT];
} Message;

static Message msg;

// Possible actions
enum ACTION {
    INSERT, CHECK_NAME, CHECK_DEPARTMENT, CHECK_SALARY, CHECK_EMPLOYEE_NUMBER, CHECK, DELETE, EXIT, ERROR
};

static const char *PROCEDURE[] = {
    "insert", "check_name", "check_department", "check_salary", "check_employee_number", "check", "delete", "exit", "error"
};

// Parse procedure type from standard in
enum ACTION get_procedure() {
    printf("Enter a procedure call: ");
    char buffer[BUFSIZ];
    fgets(buffer, BUFSIZ, stdin);
    if (strncmp(buffer, PROCEDURE[INSERT], 6) == 0) {
        return INSERT;
    } else if (strncmp(buffer, PROCEDURE[CHECK_NAME], 10) == 0) {
        return CHECK_NAME;
    } else if (strncmp(buffer, PROCEDURE[CHECK_DEPARTMENT], 16) == 0) {
        return CHECK_DEPARTMENT;
    } else if (strncmp(buffer, PROCEDURE[CHECK_SALARY], 12) == 0) {
        return CHECK_SALARY;
    } else if (strncmp(buffer, PROCEDURE[CHECK_EMPLOYEE_NUMBER], 21) == 0) {
        return CHECK_EMPLOYEE_NUMBER;
    } else if (strncmp(buffer, PROCEDURE[CHECK], 5) == 0) {
        return CHECK;
    } else if (strncmp(buffer, PROCEDURE[DELETE], 6) == 0) {
        return DELETE;
    } else if (strncmp(buffer, PROCEDURE[EXIT], 4) == 0) {
        return EXIT;
    }
    return ERROR;
}

#define ADMIN_KEY 1235
// Create admin message queue
int create_admin_msg_queue() {
    int msgid;
    msgid = msgget((key_t)ADMIN_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    return msgid;
}

// Send message
void send_msg(int msgid) {
    msg.type = 1;
    if (msgsnd(msgid, (void *)&msg, MAX_TEXT, 0) == -1) {
        fprintf(stderr, "msgsnd failed\n");
        exit(EXIT_FAILURE);
    }
}

// Receive message
char* rcv_msg(int msgid) {
    long int msg_to_receive = 0;
    if (msgrcv(msgid, (void *)&msg, BUFSIZ, msg_to_receive, 0) == -1) {
        fprintf(stderr, "msgrcv failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    return msg.payload;
}

#define BOOKKEEPER_KEY 4313
// Create bookkepper message queue
int create_bookkeeper_msg_queue() {
    int msgid;
    msgid = msgget((key_t)BOOKKEEPER_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    return msgid;
}

#endif /* common_h */
