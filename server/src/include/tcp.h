#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef TCP_H
#define TCP_H

typedef struct sockaddr_in sa_in;
typedef struct sockaddr sa;

#define BUFFER_SIZE 256
#define PORT 8093
#define MAX_PENDING 10

long socket_init();
void server_set_addr(sa_in *addr, int port);
void server_socket_bind(long sd, sa_in addr);
void server_socket_listen(long sd);

#endif