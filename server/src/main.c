#include "include/chatroom.h"
#include "include/tcp.h"
#include "include/user.h"

static bool alive = true;

static void server_start(long sd);
static void *handle_client(void *_sd);
static int register_user(long sd);
static int register_chatroom(long sd, int user_idx);
static bool parse_client_input(char *input_buffer, char *output_buffer);

int main() {
    long sd;
    sa_in my_addr;

    user_init();
    chatroom_init();

    sd = socket_init();

    server_set_addr(&my_addr, PORT);

    server_socket_bind(sd, my_addr);

    server_socket_listen(sd);

    server_start(sd);

    close(sd);
    return 0;
}

static void server_start(long sd) {
    pthread_t client_thread;
    long client_sd;
    int return_code;

    printf("server started\n");

    while (alive) {
        printf("listening for connections...\n");
        client_sd = accept(sd, 0, 0);

        if (client_sd == -1) {
            printf("failed to accept incoming connection.\n");
            alive = false;
            break;
        }

        printf("client connected...\n");

        return_code = pthread_create(&client_thread, NULL, handle_client, (void *)client_sd);

        if (return_code != 0) {
            printf("unable to create new thread.\n");
            close(client_sd);
            alive = false;
            break;
        }
    }
}

static void *handle_client(void *_sd) {
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
            if (!parse_client_input(in_buffer, out_buffer))
                break;

            strcpy(tmp_buffer, out_buffer);
            snprintf(out_buffer, BUFFER_SIZE - strlen(users[user_idx].name), "%s : %s\n", users[user_idx].name, tmp_buffer);

            chatroom_send(user_idx, room_idx, out_buffer);
        } else {
            break;
        }
    }

    snprintf(out_buffer, BUFFER_SIZE, "%s disconnected.\n", users[user_idx].name);

    chatroom_send(user_idx, room_idx, out_buffer);
    chatroom_leave(user_idx, room_idx);
    user_logout(user_idx);

    close(sd);

    return NULL;
}

static int register_user(long sd) {
    size_t bytes_recieved = 0;
    int user_idx = INVALID_USER;
    char error_buffer[BUFFER_SIZE], in_buffer[BUFFER_SIZE];
    char msg[] = "enter username\n";

    send(sd, msg, strlen(msg), 0);

    while (user_idx == INVALID_USER) {
        bytes_recieved = recv(sd, in_buffer, BUFFER_SIZE - 1, 0);

        if (bytes_recieved == 0) {
            user_idx = INVALID_USER;
            break;
        }

        user_idx = user_login(in_buffer, sd, error_buffer, BUFFER_SIZE);
        if (user_idx == INVALID_USER)
            send(sd, error_buffer, strlen(error_buffer), 0);
    }

    return user_idx;
}

static int register_chatroom(long sd, int user_idx) {
    size_t bytes_recieved = 0;
    int room_idx = INVALID_ROOM;
    char error_buffer[BUFFER_SIZE], in_buffer[BUFFER_SIZE];
    char msg[] = "enter room name\n";

    send(sd, msg, strlen(msg), 0);

    while (room_idx == INVALID_ROOM) {
        bytes_recieved = recv(sd, in_buffer, BUFFER_SIZE - 1, 0);

        if (bytes_recieved == 0) {
            room_idx = INVALID_ROOM;
            break;
        }

        room_idx = chatroom_join(user_idx, in_buffer, error_buffer, BUFFER_SIZE);
        if (room_idx == INVALID_ROOM)
            send(sd, error_buffer, strlen(error_buffer), 0);
    }

    return room_idx;
}

static bool parse_client_input(char *in_buffer, char *out_buffer) {
    if (strncmp(in_buffer, "exit", 4) == 0)
        return false;
    else
        strcpy(out_buffer, in_buffer);

    return true;
}