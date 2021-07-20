#include "include/tcp.h"

long socket_init() {
    long sd;

    sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sd == -1) {
        perror("unable to initialize socket.\n");
        exit(1);
    }

    return sd;
}

void client_set_addr(sa_in *addr, char *ip, int port) {
    memset(addr, 0, sizeof(sa_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(port);
}

void client_socket_connect(long sd, sa_in addr) {
    if (connect(sd, (sa *)&addr, sizeof(addr)) == -1) {
        perror("failed to connect to server.\n");
        close(sd);
        exit(1);
    }
}
