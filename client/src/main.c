#include "include/tcp.h"

static bool alive = true;

static void client_start(long sd);
static bool server_send(long sd);
static void *server_recieve(void *_sd);
static void read_input(char *buffer, int buffer_size);
static void overwrite_stdout();

int main() {
    long sd;
    sa_in other_addr;

    sd = socket_init();

    client_set_addr(&other_addr, ADDRESS, PORT);

    client_socket_connect(sd, other_addr);

    client_start(sd);

    close(sd);
    return 0;
}

static void client_start(long sd) {
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

static bool server_send(long sd) {
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

static void *server_recieve(void *_sd) {
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

static void read_input(char *buffer, int buffer_size) {
    fgets(buffer, buffer_size, stdin);
    buffer[strlen(buffer) - 1] = '\0';
}

static void overwrite_stdout() {
    printf("%s", "> ");
    fflush(stdout);
}
