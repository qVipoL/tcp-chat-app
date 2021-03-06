#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef USER_H
#define USER_H

#define INVALID_USER -1
#define MAX_NAME_LENGTH 10
#define MIN_NAME_LENGTH 4
#define MAX_USERS 10

typedef struct
{
    int sd;
    bool connected;
    char name[MAX_NAME_LENGTH + 1];
} user;

void user_init();
int user_find(long sd);
int user_login(const char *client_name, int sd, char *error_buffer, int error_buffer_size);
void user_logout(int user_idx);
void user_remove_connections(int sd);
user *get_users();

#endif
