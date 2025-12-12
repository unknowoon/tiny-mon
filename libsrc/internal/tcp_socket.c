#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "core/tcp_socket.h"

// 논블로킹 모드 설정
int tcp_socket_set_nonblocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// TCP 서버 소켓 생성
int tcp_socket_create_server(const char *ip, int port, struct sockaddr_in *addr) {
	// 소켓 생성
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1) {
		perror("socket");
		return -1;
	}

	// SO_REUSEADDR 설정
	int opt = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// 주소 설정
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);

	if (strcmp(ip, "0.0.0.0") == 0 || strcmp(ip, "INADDR_ANY") == 0) {
		addr->sin_addr.s_addr = INADDR_ANY;
	} else {
		if (inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
			fprintf(stderr, "Invalid IP address\n");
			close(listen_fd);
			return -1;
		}
	}

	// 바인드
	if (bind(listen_fd, (struct sockaddr*)addr, sizeof(*addr)) == -1) {
		perror("bind");
		close(listen_fd);
		return -1;
	}

	// 리슨
	if (listen(listen_fd, SOMAXCONN) == -1) {
		perror("listen");
		close(listen_fd);
		return -1;
	}

	// 논블로킹 설정
	if (tcp_socket_set_nonblocking(listen_fd) == -1) {
		perror("set_nonblocking");
		close(listen_fd);
		return -1;
	}

	return listen_fd;
}

// 클라이언트 연결 수락
int tcp_socket_accept(int listen_fd, struct sockaddr_in *client_addr) {
	socklen_t client_len = sizeof(*client_addr);
	return accept(listen_fd, (struct sockaddr*)client_addr, &client_len);
}
