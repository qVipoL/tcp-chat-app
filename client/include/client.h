#include "../../tcp/include/tcp.h"

#ifndef CLIENT_H
#define CLIENT_H

void client_start(long sd);
bool server_send(long sd);
void *server_recieve(void *_sd);
void read_input(char *buffer, int buffer_size);
void overwrite_stdout();

#endif