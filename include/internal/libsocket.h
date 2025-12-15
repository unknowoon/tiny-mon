//
// Created by 우지범 on 2025. 12. 15..
//

#ifndef QUEKKA_LIBSOCKET_H
#define QUEKKA_LIBSOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>

int socket_init(void);
void socket_bind_address(int port, int socket_fd);

void socket_set_server_address(struct sockaddr_in *server_address, const char *server_ip, int server_port);
void socket_connect(struct sockaddr_in *server_address, int socket_fd);

#endif //QUEKKA_LIBSOCKET_H