#include "../include/server.h"

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
