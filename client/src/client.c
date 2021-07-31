#include "../include/client.h"

static volatile bool alive = true;

void client_start(long sd) {
    pthread_t recv_thread;

    if (pthread_create(&recv_thread, NULL, server_recieve, (void *)sd) == -1) {
        perror("unable to create new thread.\n");
        close(sd);
        exit(1);
    }

    if (!server_send(sd)) {
        close(sd);
        exit(1);
    }
}

bool server_send(long sd) {
    char out_buffer[BUFFER_SIZE];

    while (alive) {
        overwrite_stdout();
        read_input(out_buffer, BUFFER_SIZE);

        if (send(sd, out_buffer, strlen(out_buffer) + 1, 0) == -1) {
            printf("failed to send to server.\n");
            alive = false;
            return false;
        }

        if (strcmp(out_buffer, "exit") == 0) {
            alive = false;
            break;
        }
    }

    return true;
}

void *server_recieve(void *_sd) {
    long sd;
    char in_buffer[BUFFER_SIZE];
    size_t bytes_recieved = 0;

    sd = (long)_sd;

    while (alive) {
        memset(in_buffer, 0, BUFFER_SIZE);
        bytes_recieved = recv(sd, in_buffer, BUFFER_SIZE - 1, 0);

        if (bytes_recieved > 0) {
            printf("%s", in_buffer);
            overwrite_stdout();
        } else {
            printf("lost connection with server.\n");
            alive = false;
        }
    }

    return NULL;
}

void read_input(char *buffer, int buffer_size) {
    char *tmp = fgets(buffer, buffer_size, stdin);

    if (!tmp) {
        perror("invalid input.\n");
        exit(1);
    }

    buffer[strlen(buffer) - 1] = '\0';
}

void overwrite_stdout() {
    printf("%s", "> ");
    fflush(stdout);
}