//
// Created by 우지범 on 2025. 12. 15..
//

#ifndef QUEKKA_LIBSOCKET_H
#define QUEKKA_LIBSOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

// 서버 소켓 생성/설정
int socket_init(void);
void socket_setsockopt_reuseaddr(int server_fd);
void socket_bind_address(int port, int socket_fd);
void socket_listen(int socket_fd);

// 클라이언트 연결 처리
void socket_connect_with_client(int server_fd);

// TCP 소켓 유틸
int tcp_socket_set_nonblocking(int fd);
int tcp_socket_accept(int listen_fd, struct sockaddr_in *client_addr);

// 클라이언트용
void socket_set_server_address(struct sockaddr_in *server_address, const char *server_ip, int server_port);
void socket_connect(struct sockaddr_in *server_address, int socket_fd);

#ifdef __cplusplus
}
#endif

#endif //QUEKKA_LIBSOCKET_H