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
#define ADDRESS "127.0.0.1"
#define MAX_PENDING 10

long socket_init();
void client_set_addr(sa_in *addr, char *ip, int port);
void client_socket_connect(long sd, sa_in addr);

#endif