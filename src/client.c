#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/error.h"

#define BUFFER_SIZE 256
#define PORT 8087
#define ADDRESS "127.0.0.1"

static bool alive = true;

typedef struct sockaddr_in sa_in;
typedef struct sockaddr sa;

bool server_send(long sd);
void *server_listen(void *_sd);

int main() {
    long server_sd;
    int return_code;
    sa_in server_addr;
    pthread_t listen_thread;

    server_sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    exit_with_error("unable to initialize socket.\n", server_sd, -1);

    memset(&server_addr, 0, sizeof(sa_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ADDRESS);
    server_addr.sin_port = htons(PORT);

    if (connect(server_sd, (sa *)&server_addr, sizeof(server_addr)) == -1) {
        perror("unable to connect to server.\n");
        close(server_sd);
        exit(1);
    }

    return_code = pthread_create(&listen_thread, NULL, server_listen, (void *)server_sd);

    exit_with_error("unable to create new thread.\n", return_code, -1);

    if (!server_send(server_sd)) {
        perror("failed.\n");
        close(server_sd);
        exit(1);
    }

    close(server_sd);
    return 0;
}

bool server_send(long sd) {
    char out_buffer[BUFFER_SIZE];

    while (alive) {
        // TODO: change to protected input
        scanf("%s", out_buffer);

        if (send(sd, out_buffer, strlen(out_buffer), 0) == -1) {
            perror("unable to send to server.\n");
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

void *server_listen(void *_sd) {
    long sd;
    char in_buffer[BUFFER_SIZE];
    size_t bytes_recieved = 0;

    sd = (long)_sd;

    while (alive) {
        memset(in_buffer, 0, BUFFER_SIZE);
        bytes_recieved = recv(sd, in_buffer, BUFFER_SIZE - 1, 0);

        if (bytes_recieved > 0)
            printf("%s", in_buffer);
    }

    return NULL;
}
