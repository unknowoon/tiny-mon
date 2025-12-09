#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <netinet/in.h>

// 논블로킹 모드 설정
int tcp_socket_set_nonblocking(int fd);

// TCP 서버 소켓 생성
int tcp_socket_create_server(const char *ip, int port, struct sockaddr_in *addr);

// 클라이언트 연결 수락
int tcp_socket_accept(int listen_fd, struct sockaddr_in *client_addr);

#endif
