#include "../include/user.h"

static user users[MAX_USERS];

static pthread_mutex_t users_mutex = PTHREAD_MUTEX_INITIALIZER;

void user_init() {
    int idx;

    for (idx = 0; idx < MAX_USERS; idx++) {
        users[idx].connected = false;
        users[idx].sd = -1;
        users[idx].name[0] = '\0';
    }
}

int user_find(long sd) {
    int idx;

    for (idx = 0; idx < MAX_USERS; idx++) {
        if (users[idx].sd == sd)
            return idx;
    }

    return -1;
}

int user_login(const char *client_name, int sd, char *error_buffer, int error_buffer_size) {
    int idx, name_length;

    name_length = strlen(client_name);

    if (name_length > MAX_NAME_LENGTH) {
        snprintf(error_buffer, error_buffer_size, "username too long (max: %d chars).\n", MAX_NAME_LENGTH);

        return INVALID_USER;
    } else if (name_length < MIN_NAME_LENGTH) {
        snprintf(error_buffer, error_buffer_size, "username too short (min: %d chars).\n", MIN_NAME_LENGTH);

        return INVALID_USER;
    }

    pthread_mutex_lock(&users_mutex);

    for (idx = 0; idx < MAX_USERS; idx++)
        if (!users[idx].connected)
            break;

    if (idx >= MAX_USERS) {
        pthread_mutex_unlock(&users_mutex);
        strncpy(error_buffer, "chat server is full.\n", error_buffer_size - 1);

        return INVALID_USER;
    }

    users[idx].connected = true;
    users[idx].sd = sd;
    strncpy(users[idx].name, client_name, MAX_NAME_LENGTH);

    pthread_mutex_unlock(&users_mutex);

    return idx;
}

void user_logout(int user_idx) {
    pthread_mutex_lock(&users_mutex);

    users[user_idx].connected = false;
    users[user_idx].sd = -1;
    users[user_idx].name[0] = '\0';

    pthread_mutex_unlock(&users_mutex);
}

user *get_users() {
    return users;
}