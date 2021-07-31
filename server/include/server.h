#ifndef SERVER_H
#define SERVER_H

#include "../../tcp/include/tcp.h"
#include "chatroom.h"
#include "user.h"

void server_start(long sd);
int handle_client(long sd);
bool parse_client_input(char *input_buffer, char *output_buffer);

#endif