#include <stdlib.h>

#include "internal/client_manager.h"

// 내부 클라이언트 노드 구조체
typedef struct client_node_t {
	client_info_t info;
	struct client_node_t *next;
} client_node_t;

// 클라이언트 매니저 구조체
struct client_manager_t {
	client_node_t *clients;
	int client_count;
};

// 클라이언트 매니저 생성
client_manager_t* client_manager_create(void) {
	client_manager_t *mgr = calloc(1, sizeof(client_manager_t));
	if (!mgr) return NULL;

	mgr->clients = NULL;
	mgr->client_count = 0;

	return mgr;
}

// 클라이언트 매니저 삭제
void client_manager_destroy(client_manager_t *mgr) {
	if (!mgr) return;

	// 모든 클라이언트 노드 해제
	client_node_t *node = mgr->clients;
	while (node) {
		client_node_t *next = node->next;
		free(node);
		node = next;
	}

	free(mgr);
}

// 클라이언트 추가
client_info_t* client_manager_add(client_manager_t *mgr, int fd, struct sockaddr_in addr) {
	if (!mgr) return NULL;

	if (mgr->client_count >= CLIENT_MAX_CLIENTS) {
		return NULL;
	}

	client_node_t *node = malloc(sizeof(client_node_t));
	if (!node) return NULL;

	node->info.fd = fd;
	node->info.addr = addr;
	node->info.user_data = NULL;

	// 리스트 앞에 추가
	node->next = mgr->clients;
	mgr->clients = node;
	mgr->client_count++;

	return &node->info;
}

// 클라이언트 찾기
client_info_t* client_manager_find(client_manager_t *mgr, int fd) {
	if (!mgr) return NULL;

	client_node_t *node = mgr->clients;
	while (node) {
		if (node->info.fd == fd) {
			return &node->info;
		}
		node = node->next;
	}
	return NULL;
}

// 클라이언트 삭제
void client_manager_remove(client_manager_t *mgr, int fd) {
	if (!mgr) return;

	client_node_t **pp = &mgr->clients;

	while (*pp) {
		if ((*pp)->info.fd == fd) {
			client_node_t *to_delete = *pp;
			*pp = (*pp)->next;
			free(to_delete);
			mgr->client_count--;
			return;
		}
		pp = &(*pp)->next;
	}
}

// 클라이언트 수 반환
int client_manager_count(client_manager_t *mgr) {
	if (!mgr) return 0;
	return mgr->client_count;
}
