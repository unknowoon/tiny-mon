#include "quekka/comm.h"
#include <stdlib.h>
#include <stdio.h>

int comm_init(int socket_fd) {
    if (socket_fd < 0) {
        log_error("Comm: Invalid socket file descriptor");
        return -1;
    }
    
    log_info("Comm: Initialized with socket fd %d (PID: %d)", socket_fd, getpid());
    return 0;
}

int comm_handle_data(int socket_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received, bytes_sent;
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        
        bytes_received = read(socket_fd, buffer, BUFFER_SIZE - 1);
        
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                log_info("Comm: Client disconnected");
            } else {
                log_error("Comm: read failed: %s", strerror(errno));
            }
            break;
        }
        
        buffer[bytes_received] = '\0';
        log_debug("Comm: Received %ld bytes: %s", bytes_received, buffer);
        
        if (strncmp(buffer, "quit", 4) == 0) {
            log_info("Comm: Client requested to quit");
            break;
        }
        
        char response[BUFFER_SIZE];
        snprintf(response, BUFFER_SIZE, "Echo: %s", buffer);
        
        bytes_sent = write(socket_fd, response, strlen(response));
        if (bytes_sent < 0) {
            log_error("Comm: write failed: %s", strerror(errno));
            break;
        }
        
        log_debug("Comm: Sent %ld bytes: %s", bytes_sent, response);
    }
    
    return 0;
}

void comm_cleanup(int socket_fd) {
    log_info("Comm: Socket closed");
    close(socket_fd);
}

#if 0
int main(int argc, char *argv[]) {
    char log_filename[256];
    int socket_fd;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <socket_fd>\n", argv[0]);
        exit(1);
    }
    
    socket_fd = atoi(argv[1]);
    
    snprintf(log_filename, sizeof(log_filename), "comm_%d.log", getpid());
    logger_init(log_filename, LOG_INFO);
    log_info("Comm: Starting child process %d", getpid());
    
    comm_init(socket_fd);
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    if (getpeername(socket_fd, (struct sockaddr *)&client_addr, &client_len) == 0) {
        log_info("Comm: Handling client %s:%d", 
               inet_ntoa(client_addr.sin_addr), 
               ntohs(client_addr.sin_port));
    } else {
        log_info("Comm: Handling client (address unknown)");
    }
    
    comm_handle_data(socket_fd);
    comm_cleanup(socket_fd);
    
    log_info("Comm: Process %d terminating", getpid());
    logger_close();
    return 0;
}
#endif