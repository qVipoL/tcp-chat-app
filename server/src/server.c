#include "../include/server.h"

static volatile bool alive = true;

void server_start(long sd) {
    long client_sd;
    int i, sd_max;
    fd_set active_fds, read_fds;

    FD_ZERO(&active_fds);
    FD_ZERO(&read_fds);

    FD_SET(sd, &active_fds);

    sd_max = sd;

    printf("server started.\n");

    while (alive) {
        read_fds = active_fds;

        if (select(sd_max + 1, &read_fds, NULL, NULL, NULL) == -1) {
            alive = false;
            printf("failed to select.\n");
            break;
        }

        for (i = 0; i <= sd_max; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == sd) {
                    client_sd = accept(sd, 0, 0);

                    printf("client connected...\n");

                    if (client_sd == -1) {
                        printf("failed to accept incoming connection.\n");
                        alive = false;
                        break;
                    }

                    send(client_sd, "enter username\n", 17, 0);

                    FD_SET(client_sd, &active_fds);

                    if (client_sd > sd_max)
                        sd_max = client_sd;
                } else {
                    if (handle_client(i) == -1)
                        FD_CLR(i, &active_fds);
                }
            }
        }
    }
}

int handle_client(long sd) {
    int user_idx, room_idx;
    char in_buffer[BUFFER_SIZE], out_buffer[BUFFER_SIZE + 20], error_buffer[BUFFER_SIZE], tmp_buffer[BUFFER_SIZE];
    size_t bytes_recieved;
    user *users;

    bytes_recieved = recv(sd, in_buffer, BUFFER_SIZE, 0);
    users = get_users();
    user_idx = user_find(sd);
    room_idx = user_idx != -1 ? chatroom_user_find(user_idx) : -1;

    if (bytes_recieved <= 0) {
        if (room_idx != -1) {
            sprintf(out_buffer, "%s disconnected\n", users[user_idx].name);
            chatroom_send(user_idx, room_idx, out_buffer);
        }

        return -1;
    }

    if (user_idx == -1) {
        user_idx = user_login(in_buffer, sd, error_buffer, BUFFER_SIZE);

        if (user_idx == INVALID_USER) {
            send(sd, error_buffer, strlen(error_buffer), 0);
        } else {
            send(sd, "enter chatroom\n", 17, 0);
        }

        return 0;
    }

    if (room_idx == -1) {
        room_idx = chatroom_join(user_idx, in_buffer, error_buffer, BUFFER_SIZE);

        if (room_idx == INVALID_ROOM) {
            send(sd, error_buffer, strlen(error_buffer), 0);
        } else {
            sprintf(out_buffer, "%s connected\n", users[user_idx].name);
            chatroom_send(-1, room_idx, out_buffer);
        }

        return 0;
    }

    if (!parse_client_input(in_buffer, out_buffer)) {
        sprintf(out_buffer, "%s disconnected\n", users[user_idx].name);
        chatroom_send(user_idx, room_idx, out_buffer);

        return -1;
    }

    strcpy(tmp_buffer, out_buffer);
    sprintf(out_buffer, "%s : %s\n", users[user_idx].name, tmp_buffer);

    chatroom_send(user_idx, room_idx, out_buffer);

    return 0;
}

bool parse_client_input(char *in_buffer, char *out_buffer) {
    if (strncmp(in_buffer, "exit", 4) == 0)
        return false;
    else
        strcpy(out_buffer, in_buffer);

    return true;
}