#include "../include/chatroom.h"

#include "../../tcp/include/tcp.h"
#include "../include/user.h"

static chatroom rooms[MAX_ROOMS];

static pthread_mutex_t rooms_mutex = PTHREAD_MUTEX_INITIALIZER;

void chatroom_init() {
    int idx, jdx;

    for (idx = 0; idx < MAX_ROOMS; idx++) {
        rooms[idx].active = false;
        rooms[idx].name[0] = '\0';

        for (jdx = 0; jdx < MAX_USERS_PER_ROOM; jdx++)
            rooms[idx].users_idxs[jdx] = INVALID_USER;
    }
}

int chatroom_user_find(int user_idx) {
    int idx, jdx;

    for (idx = 0; idx < MAX_ROOMS; idx++)
        for (jdx = 0; jdx < MAX_USERS_PER_ROOM; jdx++)
            if (rooms[idx].users_idxs[jdx] == user_idx)
                return idx;

    return -1;
}

int chatroom_join(int user_idx, const char *room_name, char *error_buffer, int error_buffer_size) {
    int idx, jdx;

    if (strlen(room_name) > MAX_ROOMNAME_LENGTH) {
        snprintf(error_buffer, error_buffer_size, "room name too long (max: %d chars).\n", MAX_ROOMNAME_LENGTH);

        return INVALID_ROOM;
    }

    if (strlen(room_name) < MIN_ROOMNAME_LENGTH) {
        snprintf(error_buffer, error_buffer_size, "room name too too short (min: %d chars).\n", MIN_ROOMNAME_LENGTH);

        return INVALID_ROOM;
    }

    pthread_mutex_lock(&rooms_mutex);

    for (idx = 0; idx < MAX_ROOMS; idx++) {
        if (strcmp(rooms[idx].name, room_name) == 0) {
            for (jdx = 0; jdx < MAX_USERS_PER_ROOM; jdx++) {
                if (rooms[idx].users_idxs[jdx] == INVALID_USER) {
                    rooms[idx].users_idxs[jdx] = user_idx;

                    break;
                }
            }

            break;
        }
    }

    if (idx == MAX_ROOMS) {
        for (idx = 0; idx < MAX_ROOMS; idx++) {
            if (!rooms[idx].active) {
                rooms[idx].active = true;
                strcpy(rooms[idx].name, room_name);

                for (jdx = 0; jdx < MAX_USERS_PER_ROOM; jdx++)
                    rooms[idx].users_idxs[jdx] = INVALID_USER;

                rooms[idx].users_idxs[0] = user_idx;

                break;
            }
        }
    }

    pthread_mutex_unlock(&rooms_mutex);

    if (idx == MAX_ROOMS) {
        strncpy(error_buffer, "chat room is full.\n", error_buffer_size);

        return INVALID_ROOM;
    }

    return idx;
}

void chatroom_leave(int user_index, int room_idx) {
    int idx;
    chatroom room;

    pthread_mutex_lock(&rooms_mutex);

    room = rooms[room_idx];

    for (idx = 0; idx < MAX_USERS_PER_ROOM; ++idx)
        if (room.users_idxs[idx] == user_index)
            room.users_idxs[idx] = -1;

    pthread_mutex_unlock(&rooms_mutex);
}

void chatroom_send(int user_idx, int chatroom_idx, const char *msg) {
    int idx;
    chatroom room;
    user *users;

    users = get_users();

    room = rooms[chatroom_idx];

    for (idx = 0; idx < MAX_USERS_PER_ROOM; idx++) {
        if (room.users_idxs[idx] != INVALID_USER && room.users_idxs[idx] != user_idx) {
            idx = room.users_idxs[idx];

            send(users[idx].sd, msg, strlen(msg), 0);
        }
    }
}

chatroom *get_rooms() {
    return rooms;
}
