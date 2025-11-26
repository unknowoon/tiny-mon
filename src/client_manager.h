#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <netinet/in.h>

#define CLIENT_MAX_CLIENTS 1000

// 클라이언트 정보
typedef struct {
	int fd;
	struct sockaddr_in addr;
	void *user_data;
} client_info_t;

// 불투명 포인터
typedef struct client_manager_t client_manager_t;

// 클라이언트 매니저 생성
client_manager_t* client_manager_create(void);

// 클라이언트 매니저 삭제
void client_manager_destroy(client_manager_t *mgr);

// 클라이언트 추가
client_info_t* client_manager_add(client_manager_t *mgr, int fd, struct sockaddr_in addr);

// 클라이언트 찾기
client_info_t* client_manager_find(client_manager_t *mgr, int fd);

// 클라이언트 삭제
void client_manager_remove(client_manager_t *mgr, int fd);

// 클라이언트 수 반환
int client_manager_count(client_manager_t *mgr);

#endif
