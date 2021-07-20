#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#ifndef CHATROOM_H
#define CHATROOM_H

#define INVALID_ROOM -1
#define MAX_ROOMS 5
#define MAX_ROOMNAME_LENGTH 10
#define MIN_ROOMNAME_LENGTH 5
#define MAX_USERS_PER_ROOM 10

typedef struct
{
    bool active;
    char name[MAX_ROOMNAME_LENGTH + 1];
    int users_idxs[MAX_USERS_PER_ROOM];
} chatroom;

void chatroom_init();
int chatroom_join(int user_idx, const char *room_name, char *error_buffer, int error_buffer_size);
void chatroom_leave(int user_idx, int room_idx);
void chatroom_send(int user_idx, int room_idx, const char *msg);
chatroom *get_rooms();

#endif