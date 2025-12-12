//
// Created by b532882 on 11/25/25.
//

#ifndef COMMMANAGER_H
#define COMMMANAGER_H

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "internal/comm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PORT 8080
#define BACKLOG 5

int create_server_socket(int port);

void handle_client_connection(int server_fd);

#ifdef __cplusplus
}
#endif

#endif //COMMMANAGER_H
