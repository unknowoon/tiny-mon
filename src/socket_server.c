#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include "socket_server.h"
#include "tcp_socket.h"
#include "epoll_handler.h"
#include "client_manager.h"

#define BUFFER_SIZE 4096

// 내부 서버 구조체
struct socket_server_t {
	int listen_fd;
	epoll_handler_t *epoll;
	client_manager_t *client_mgr;
	struct sockaddr_in addr;
	server_callbacks_t callbacks;
	void *user_data;
	int running;
};

// === 이벤트 핸들러들 ===

// 새로운 연결 수락
static void handle_accept(socket_server_t *server) {
	while (1) {
		struct sockaddr_in client_addr;
		int client_fd = tcp_socket_accept(server->listen_fd, &client_addr);

		if (client_fd == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;  // 모든 대기 중인 연결 처리 완료
			} else {
				perror("accept");
				break;
			}
		}

		// 논블로킹 설정
		if (tcp_socket_set_nonblocking(client_fd) == -1) {
			close(client_fd);
			continue;
		}

		// 클라이언트 등록
		client_info_t *client = client_manager_add(server->client_mgr, client_fd, client_addr);
		if (!client) {
			fprintf(stderr, "Too Many Clients\n");
			close(client_fd);
			continue;
		}

		// epoll에 추가
		if (epoll_handler_add(server->epoll, client_fd, EPOLLIN | EPOLLET) == -1) {
			client_manager_remove(server->client_mgr, client_fd);
			close(client_fd);
			continue;
		}

		// on_connection 콜백 호출
		if (server->callbacks.on_connection) {
			server->callbacks.on_connection(client, server->user_data);
		}
	}
}

// 클라이언트 데이터 수신
static void handle_client(socket_server_t *server, int client_fd) {
	client_info_t *client = client_manager_find(server->client_mgr, client_fd);
	if (!client) return;

	char buffer[BUFFER_SIZE];

	while (1) {
		ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);

		if (n > 0) {
			// 데이터 수신 성공 - on_received 콜백 호출
			if (server->callbacks.on_received) {
				server->callbacks.on_received(client, buffer, n, server->user_data);
			}

		} else if (n == 0) {
			// 연결 종료
			if (server->callbacks.on_disconnected) {
				server->callbacks.on_disconnected(client, server->user_data);
			}

			epoll_handler_remove(server->epoll, client_fd);
			close(client_fd);
			client_manager_remove(server->client_mgr, client_fd);
			break;

		} else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			} else {
				perror("recv");
				if (server->callbacks.on_disconnected) {
					server->callbacks.on_disconnected(client, server->user_data);
				}
				epoll_handler_remove(server->epoll, client_fd);
				close(client_fd);
				client_manager_remove(server->client_mgr, client_fd);
				break;
			}
		}
	}
}

// epoll 이벤트 콜백
static void epoll_event_callback(int fd, uint32_t events, void *user_data) {
	socket_server_t *server = (socket_server_t*)user_data;

	if (fd == server->listen_fd) {
		handle_accept(server);
	} else {
		handle_client(server, fd);
	}
}

// === 공개 API 함수들 ===

socket_server_t* socket_server_create(
		const char *ip,
		int port,
		server_callbacks_t *callbacks,
		void *user_data) {
	if (!callbacks) {
		fprintf(stderr, "Callbacks are required\n");
		return NULL;
	}

	socket_server_t *server = calloc(1, sizeof(socket_server_t));
	if (!server) return NULL;

	// TCP 소켓 생성
	server->listen_fd = tcp_socket_create_server(ip, port, &server->addr);
	if (server->listen_fd == -1) {
		free(server);
		return NULL;
	}

	// epoll 핸들러 생성
	server->epoll = epoll_handler_create();
	if (!server->epoll) {
		close(server->listen_fd);
		free(server);
		return NULL;
	}

	// 클라이언트 매니저 생성
	server->client_mgr = client_manager_create();
	if (!server->client_mgr) {
		epoll_handler_destroy(server->epoll);
		close(server->listen_fd);
		free(server);
		return NULL;
	}

	// listen_fd를 epoll에 등록
	if (epoll_handler_add(server->epoll, server->listen_fd, EPOLLIN | EPOLLET) == -1) {
		client_manager_destroy(server->client_mgr);
		epoll_handler_destroy(server->epoll);
		close(server->listen_fd);
		free(server);
		return NULL;
	}

	server->callbacks = *callbacks;
	server->user_data = user_data;
	server->running = 0;

	return server;
}

int socket_server_start(socket_server_t *server) {
	if (!server) return -1;

	server->running = 1;

	// on_start 콜백 호출
	if (server->callbacks.on_start) {
		server->callbacks.on_start(server->user_data);
	}

	// 이벤트 루프
	while (server->running) {
		int ret = epoll_handler_wait(server->epoll, -1, epoll_event_callback, server);
		if (ret == -1 && errno != EINTR) {
			break;
		}
	}

	// on_stop 콜백 호출
	if (server->callbacks.on_stop) {
		server->callbacks.on_stop(server->user_data);
	}

	return 0;
}

void socket_server_stop(socket_server_t *server) {
	if (!server) return;
	server->running = 0;
}

void socket_server_destroy(socket_server_t *server) {
	if (!server) return;

	if (server->listen_fd != -1) {
		close(server->listen_fd);
	}

	if (server->epoll) {
		epoll_handler_destroy(server->epoll);
	}

	if (server->client_mgr) {
		client_manager_destroy(server->client_mgr);
	}

	free(server);
}

int socket_server_send(client_info_t *client, const char *data, size_t len) {
	if (!client || !data) return -1;
	return send(client->fd, data, len, 0);
}

void socket_server_disconnect(client_info_t *client) {
	if (!client) return;
	close(client->fd);
}
