#include "../include/client.h"

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