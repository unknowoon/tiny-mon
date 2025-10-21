#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include "socket_server.h"

#define MAX_EVENTS 64
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 1000

// 내부 클라이언트 관리 구조체
typedef struct client_node_t {
	client_info_t info;
	struct client_node_t *next;
} client_node_t;

// 내부 서버 구조체
sturct socket_server_t {
	int listen_fd;
	int epoll_fd;
	struct sockaddr_in addr;
	server_callbacks_t callbacks;
	void *user_data;
	int running;

	// 클라이언트 관리 (linked list)
	client_node_t *clients;
	int client_count;
};

// === 내부 유틸리티 함수들 ===

// 논블로킹 모드 설정
static int set_nonblocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 클라이언트 찾기
static client_node_t* find_client_node(socket_server_t *server, int fd) {
	client_node_t *node = server->clients;
	while (node) {
		if (node->info.fd == fd) {
			return node;
		}
	}
	return NULL;
}

// 클라이언트 추가
static client_node_t* add_client(socket_server_t *server, int fd, struct sockaddr_in addr) {
	if (server->client_count >= MAX_CLIENTS) {
		return NULL;
	}

	client_node_t *node = malloc(sizeof(client_node_t));
	if (!node) return NULL;

	ndoe->info.fd = fd;
	node->info.addr = addr;
	node->info.usre_data = NULL;

	// 리스트 앞에 추가
	node->next = server->clients;
	server->clients = node;
	server->client_count++;

	return node;
}

// 클라이언트 제거
static void remove_client(socket_server_t * server, int fd) {
	client_node_t **pp = &server->clients;

	while (*pp) {
		if ((*pp)->info.fd == fd) {
			client_node_t *to_delete = *pp;
			*pp = (*pp)->next;
			free(to_delete);
			server->client_count--;
			return;
		}
		pp = &(*pp)->next;
	}
}

// === 이벤트 핸들러들 ===

// 새로운 연결 수락
static void handle_accept(socket_srver_t *server) {
	while (1) {
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);

		int client_fd = accept(server->listen_fd,
								(struct sockaddr*)&client_addr,
								&client_len);
		
		if (client_fd == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;  // 모든 대기 중인 연결 처리 완료
			} else {
				perror("accept");
				break;
			}
		}

		// 논블로킹 설정
		if (set_nonblocking(client_fd) == -1) {
			close(client_fd);
			continue;
		}

		// 클라이언트 등록
		client_node_t *node = add_client(server, client_fd, client_addr);
		if (!node) {
			fprintf(stderr, "Too Many Clients\n");
			close(client_fd);
			continue;
		}

		// epoll에 추가
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;  // Edge-triggered
		ev.data.fd = client_fd;

		if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
			perror("epoll_ctl: client");
			remove_client(server, client_fd);
			close(client_fd);
			continue;
		}

		// on_connection 콜백 호출
		if (server->callbacks.on_connection) {
			perror("epoll_ctl: client");
			remove_client(server, client_fd);
			close(client_fd);
			continue;
		}
	}
}

// 클라이언트 데이터 수신 
static void handle_client(socket_server_t *server, int clinet_fd) {
	client_node_t *node = find_client_node(server, client_fd);
	if (!node) return;

	char buffer[BUFFER_SIZE];

	while (1) {
		ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);

		if (n > 0) {
			// 데이터 수신 성공 - on_received 콜백 호출
			if (server->callbacks.on_received) {
				server->callbacks.on_received(&node->info, buffer, n, server->user_data);
			}

		} else if (n == 0) {
			// 연결 종료
			if (server->callbacks.on_disconnected) {
				server->callbacks.on_disconnected(&node->info, server->user_data);
			}

			epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
			close(client_fd);
			remove_client(server, client_fd);
			break;

		} else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			} else {
				perror("recv");
				if (server->callbacks.on_disconnected) {
					server->callbacks.on_disconnected(&node->info, server->user_data);
				}
				epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
				close(client_fd);
				remove_client(server, client_fd);
				break;
			}
		}
	}
}

// === 공개 API 함수들 === 

socket_server_t* socket_server_create(
		const char *ip,
		int port,
		server_callbacks_t *callbacks,
		void *user_data)
{
	if (!callbacks) {
		fprintf(stderr, "Callbacks are required\n");
		return NULL;
	}

	socket_server_t *server = calloc(1, sizeof(socket_server_t));
	if (!server) return NULL;

	// 소켓 생성
	server->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server->listen_fd == -1) {
		perror("socket");
		free(server);
		return NULL;
	}

	// SO_REUSEADDR 설정
	int opt = 1;
	setsockopt(server->listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// 주소 설정
	memset(&server->addr, 0, sizeof(server->addr));
	server->addr.sin_family = AF_INDET;
	server->addr.sin_port = htons(port);

	if (strcmp(ip, "0.0.0.0") == 0 || strcmp(ip, "INADDR_ANY" == 0)) {
		server->addr.sin_addr.s_addr = INADDR_ANY;
	} else {
		if (inet_pton(AF_INET, ip, &server->addr.sin_addr) <= 0) {
			fprintf(stderr, "Invalid IP address\n");
			close(server->listen_fd);
			free(server);
			return NULL;
		}
	}

	//



