#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <stddef.h>
#include <netinet/in.h>

// 불투명 포인터
typedef struct socket_server_t socket_server_t

// 클라이언트 정보
typedef struct {
	int fd;
	struct socketaddr_in addr;
	void *user_data;
} client_info_t;

// 5가지 이벤트 콜백
typedef void (*on_start_fn)(void *user_data);
typedef void (*on_stop_fn)(void *user_data);
typedef void (*on_connection_fn)(client_info_t *client, void *user_data);
typedef void (*on_disconnected_fn)(client_info_t *client, void *user_data);
typedef void (*on_received_fn)(client_info_t *client, const char *data, size_t len, void *user_data);

// 콜백 묶음 구조체
typedef struct {
	on_start_fn on_start;
	on_stop_fn on_stop;
	on_connection_fn on_connection;
	on_disconnected_fn on_disconnected;
	on_received_fn on_received;
} server_callbacks_t;

// == API 함수들 ===

// 서버 생성
socket_server_t* socket_server_create(
	const char *ip,
	int port,
	server_callbacks_t *callbacks,
	void *user_data
);

// 서버 시작 (블로킹)
int socket_server_stawrt(socket_server_t *server);

// 서버 중지
void socket_server_stop(socket_server_t *server);

// 서버 제거
void socket_server_destroy(socket_server_t *server);

// 클라이언트에게 데이터 전송
int socket_server_send(client_info_t *client, const char *data, size_t len);

// 클라이언트 연결 종료
void socket_server_disconnect(client_info_t *client);

#endif
	
