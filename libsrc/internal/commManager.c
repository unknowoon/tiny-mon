#include "core/commManager.h"

int create_server_socket(int port) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        const int err = errno;
        log_error("socket failed: %s", strerror(err));
        exit(1);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) {
        log_error("setsockopt failed: %s", strerror(errno));
        close(server_fd);
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        log_error("bind failed: %s", strerror(errno));
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        log_error("listen failed: %s", strerror(errno));
        close(server_fd);
        exit(1);
    }

    log_info("CommManager: Server listening on port %d", port);
    return server_fd;
}

void handle_client_connection(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd;
    pid_t pid;

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            log_error("accept failed: %s", strerror(errno));
            continue;
        }

        log_info("CommManager: Client connected from %s:%d", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pid = fork();

        if (pid < 0) {
            log_error("fork failed: %s", strerror(errno));
            close(client_fd);
        } else if (pid == 0) {
            close(server_fd);
            
            char fd_str[32];
            snprintf(fd_str, sizeof(fd_str), "%d", client_fd);
            
            execl("./comm", "comm", fd_str, NULL);
            log_error("execl failed");
            exit(1);
        } else {
            close(client_fd);
            log_info("CommManager: Created child process %d for client", pid);
        }
    }
}

#if 0
int main(int argc, char *argv[]) {
    int server_fd;
    int port = PORT;

    logger_init("commmanager.log", LOG_INFO);
    log_info("CommManager: Starting up...");

    if (argc > 1) {
        port = atoi(argv[1]);
        if (port < 1 || port > 65535) {
            log_error("Invalid port number: %s", argv[1]);
            exit(1);
        }
    }

    server_fd = create_server_socket(port);
    if (server_fd < 0) {
        exit(1);
    }

    log_info("CommManager: Starting server on port %d", port);
    handle_client_connection(server_fd);

    close(server_fd);
    log_info("CommManager: Shutting down");
    logger_close();
    return 0;
}
#endif