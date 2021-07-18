#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>

#include "chatroom.h"
#include "../user/user.h"

static chatroom rooms[MAX_ROOMS];

static pthread_mutex_t rooms_mutex = PTHREAD_MUTEX_INITIALIZER;

bool chatroom_init()
{
    int i, j;

    for (i = 0; i < MAX_ROOMS; ++i)
    {
        rooms[i].active = false;
        rooms[i].name[0] = '\0';

        for (j = 0; j < MAX_USERS_PER_ROOM; ++j)
            rooms[i].users_idxs[j] = INVALID_USER;
    }

    return true;
}

int chatroom_join(int user_idx, const char *room_name, char *error_buffer, int error_buffer_size)
{
    int i, j;

    if (room_name == NULL || strlen(room_name) > MAX_ROOMNAME_LENGTH)
    {
        snprintf(error_buffer, error_buffer_size, "room name too long (max: %d chars) or too short (min: 0 chars).\n", MAX_ROOMNAME_LENGTH);

        return INVALID_ROOM;
    }

    if (user_idx == INVALID_USER)
    {
        strncpy(error_buffer, "invalid user.\n", error_buffer_size);

        return INVALID_USER;
    }

    pthread_mutex_lock(&rooms_mutex);

    for (i = 0; i < MAX_ROOMS; ++i)
    {
        if (strcmp(rooms[i].name, room_name) == 0)
        {
            for (j = 0; j < MAX_USERS_PER_ROOM; ++j)
            {
                if (rooms[i].users_idxs[j] == INVALID_USER)
                {
                    rooms[i].users_idxs[j] = user_idx;

                    break;
                }
            }

            break;
        }
    }

    if (i == MAX_ROOMS)
    {
        for (i = 0; i < MAX_ROOMS; i++)
        {
            if (!rooms[i].active)
            {
                rooms[i].active = true;
                strcpy(rooms[i].name, room_name);

                for (j = 0; j < MAX_USERS_PER_ROOM; j++)
                    rooms[i].users_idxs[j] = INVALID_USER;

                rooms[i].users_idxs[0] = user_idx;

                break;
            }
        }
    }

    pthread_mutex_unlock(&rooms_mutex);

    if (i == MAX_ROOMS)
    {
        strncpy(error_buffer, "chat room is full.\n", error_buffer_size);

        return INVALID_ROOM;
    }

    return i;
}

void chatroom_leave(int user_index, int room_idx)
{
    int i;
    chatroom room;

    pthread_mutex_lock(&rooms_mutex);

    room = rooms[room_idx];

    for (i = 0; i < MAX_USERS_PER_ROOM; ++i)
        if (room.users_idxs[i] == user_index)
            room.users_idxs[i] = -1;

    pthread_mutex_unlock(&rooms_mutex);
}

void chatroom_send(int user_idx, int chatroom_idx, const char *msg)
{
    int i, idx;
    chatroom room;
    user *users;

    users = get_users();

    pthread_mutex_lock(&rooms_mutex);

    room = rooms[chatroom_idx];

    for (i = 0; i < MAX_USERS_PER_ROOM; i++)
    {
        if (room.users_idxs[i] != user_idx)
        {
            idx = room.users_idxs[i];

            send(users[idx].sd, msg, strlen(msg), 0);
        }
    }

    pthread_mutex_unlock(&rooms_mutex);
}
