#ifndef COMM_H
#define COMM_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../../logger/inc/logger.h"

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
    int socket_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len;
} comm_context_t;

int comm_init(int socket_fd);
int comm_handle_data(int socket_fd);
void comm_cleanup(int socket_fd);

#endif /* COMM_H */