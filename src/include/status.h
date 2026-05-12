#ifndef STATUS_H
#define STATUS_H

#include <stdio.h>

typedef enum status_t {
    SUCCESS = 1,
    ERR_EMPTY = 2,
    ERR_FULL = 3,
    ERR_VALUE = 4,
    ERR_NOT_FOUND = 5,
    END_GAME = 6,
    ERR_MEM = 0,
} status_t;


void print_status(status_t status);
void print_error(status_t status);

#ifdef STATUS_IMPLEMENTATION

void print_status(status_t status) {
    if (status == SUCCESS) {
        printf("SUCCESS");
    }
    else if (status == ERR_EMPTY) {
        printf("ERR_EMPTY");
    }
    else if (status == ERR_FULL) {
        printf("ERR_FULL");
    }
    else if (status == ERR_MEM) {
        printf("ERR_MEM");
    }
    else if (status == ERR_VALUE) {
        printf("ERR_VALUE");
    }
    else if (status == ERR_NOT_FOUND) {
        printf("ERR_NOT_FOUND");
    }
}

void print_error(status_t status) {
    printf("\n(ended with ");
    print_status(status);
    printf(")\n");
}
#endif

#endif
