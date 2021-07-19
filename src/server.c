#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/chatroom.h"
#include "../include/error.h"
#include "../include/user.h"

#define BUFFER_SIZE 256
#define MAX_PENDING 10
#define PORT 8087

static bool alive = true;

typedef struct sockaddr_in sa_in;
typedef struct sockaddr sa;

int register_user(long sd);
int register_chatroom(long sd, int user_idx);
void *handle_client(void *_sd);
bool parse_input(char *input_buffer, char *output_buffer);

int main() {
    long sd, client_sd;
    int return_code;
    pthread_t client_thread;
    sa_in server_addr;

    exit_with_error("unable to initialize user table.\n", user_init(), false);
    exit_with_error("unable to initialize chatroom table.\n", chatroom_init(), false);

    sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    exit_with_error("unable to initialize socket.\n", sd, -1);

    memset(&server_addr, 0, sizeof(sa_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    exit_with_error("unable to bind socket.\n", bind(sd, (sa *)&server_addr, sizeof(sa_in)), -1);

    exit_with_error("unable to listen socket.\n", listen(sd, MAX_PENDING), -1);

    puts("server started");

    while (alive) {
        puts("listening for connections...");
        client_sd = accept(sd, 0, 0);
        puts("client connected...");

        if (client_sd == -1) {
            perror("unable to accept incoming connection.\n");
            alive = false;
            break;
        }

        return_code = pthread_create(&client_thread, NULL, handle_client, (void *)client_sd);

        if (return_code != 0) {
            perror("unable to create new thread.\n");
            close(client_sd);
            alive = false;
            break;
        }
    }

    close(sd);
    return 0;
}

int register_user(long sd) {
    size_t bytes_recieved = 0;
    int user_idx;
    char error_buffer[BUFFER_SIZE], in_buffer[BUFFER_SIZE];
    char msg[] = "enter username\n";

    send(sd, msg, strlen(msg), 0);

    while (bytes_recieved == 0)
        bytes_recieved = recv(sd, in_buffer, BUFFER_SIZE - 1, 0);

    user_idx = user_login(in_buffer, sd, error_buffer, BUFFER_SIZE);

    if (user_idx == INVALID_USER)
        send(sd, error_buffer, strlen(error_buffer), 0);

    return user_idx;
}

int register_chatroom(long sd, int user_idx) {
    size_t bytes_recieved = 0;
    int room_idx;
    char error_buffer[BUFFER_SIZE], in_buffer[BUFFER_SIZE];
    char msg[] = "enter room name\n";

    send(sd, msg, strlen(msg), 0);

    while (bytes_recieved == 0)
        bytes_recieved = recv(sd, in_buffer, BUFFER_SIZE - 1, 0);

    room_idx = chatroom_join(user_idx, in_buffer, error_buffer, BUFFER_SIZE);

    if (room_idx == INVALID_ROOM)
        send(sd, error_buffer, strlen(error_buffer), 0);

    return room_idx;
}

void *handle_client(void *_sd) {
    char in_buffer[BUFFER_SIZE], out_buffer[BUFFER_SIZE], tmp_buffer[BUFFER_SIZE];
    size_t bytes_recieved = 0;
    int user_idx, room_idx;
    user *users;
    long sd;

    sd = (long)_sd;
    users = get_users();

    user_idx = register_user(sd);
    if (user_idx == INVALID_USER)
        return (void *)INVALID_USER;

    room_idx = register_chatroom(sd, user_idx);
    if (room_idx == INVALID_ROOM)
        return (void *)INVALID_ROOM;

    snprintf(out_buffer, BUFFER_SIZE, "%s connected.\n", users[user_idx].name);
    chatroom_send(-1, room_idx, out_buffer);

    while (alive) {
        memset(in_buffer, 0, BUFFER_SIZE);
        bytes_recieved = recv(sd, in_buffer, BUFFER_SIZE - 1, 0);

        if (bytes_recieved > 0) {
            if (!parse_input(in_buffer, out_buffer))
                break;

            strcpy(tmp_buffer, out_buffer);
            snprintf(out_buffer, BUFFER_SIZE - strlen(users[user_idx].name), "%s : %s\n", users[user_idx].name, tmp_buffer);

            chatroom_send(user_idx, room_idx, out_buffer);
        }
    }

    snprintf(out_buffer, BUFFER_SIZE, "%s disconnected.\n", users[user_idx].name);

    chatroom_send(user_idx, room_idx, out_buffer);
    chatroom_leave(user_idx, room_idx);
    user_logout(user_idx);

    close(sd);

    return NULL;
}

bool parse_input(char *in_buffer, char *out_buffer) {
    if (strncmp(in_buffer, "exit", 4) == 0)
        return false;

    strcpy(out_buffer, in_buffer);

    return true;
}