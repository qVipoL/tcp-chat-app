#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "user.h"
#include "../error/error.h"

static user users[MAX_USERS];

static pthread_mutex_t users_mutex = PTHREAD_MUTEX_INITIALIZER;

bool user_init()
{
    int i;

    for (i = 0; i < MAX_USERS; i++)
    {
        users[i].connected = false;
        users[i].sd = -1;
        users[i].name[0] = '\0';
    }

    return true;
}

int user_login(const char *client_name, int sd, char *error_buffer, int error_buffer_size)
{
    int i;

    if (strlen(client_name) > MAX_NAME_LENGTH)
    {
        snprintf(error_buffer, error_buffer_size, "username too long (max: %d chars).\n", MAX_NAME_LENGTH);

        return INVALID_USER;
    }

    pthread_mutex_lock(&users_mutex);

    for (i = 0; i < MAX_USERS; i++)
        if (!users[i].connected)
            break;

    if (i >= MAX_USERS)
    {
        pthread_mutex_unlock(&users_mutex);
        strncpy(error_buffer, "chat server is full.\n", error_buffer_size - 1);

        return INVALID_USER;
    }

    users[i].connected = true;
    users[i].sd = sd;
    strncpy(users[i].name, client_name, MAX_NAME_LENGTH);

    pthread_mutex_unlock(&users_mutex);

    return i;
}

void user_logout(int user_idx)
{
    pthread_mutex_lock(&users_mutex);

    users[user_idx].connected = false;
    users[user_idx].sd = -1;
    users[user_idx].name[0] = '\0';

    pthread_mutex_unlock(&users_mutex);
}

void user_remove_connections(int sd)
{
    int i;

    pthread_mutex_lock(&users_mutex);

    for (i = 0; i < MAX_USERS; i++)
    {
        if (users[i].sd == sd)
        {
            users[i].connected = false;
            users[i].sd = -1;
            users[i].name[0] = 0;
        }
    }

    pthread_mutex_unlock(&users_mutex);
}

user *get_users()
{
    return users;
}