#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>

#include "quekka/socket_server.h"

// 전역 서버 포인터 (시그널 핸들러용)
static socket_server_t *g_server = NULL;

// 시그널 핸들러
void signal_handler(int sig) {
	printf("\nReceived signal %d, stopping server...\n", sig);
    if (g_server) {
        socket_server_stop(g_server);
    }
}

// === 5가지 콜백 함수 구현 ===

void on_start(void *user_data) {
    printf("🚀 [EVENT] Server started!\n");
}

void on_stop(void *user_data) {
    printf("🛑 [EVENT] Server stopped!\n");
}

void on_connection(client_info_t *client, void *user_data) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client->addr.sin_addr, ip, sizeof(ip));

    printf("✅ [EVENT] Client connected: %s:%d (fd=%d)\n",
           ip, ntohs(client->addr.sin_port), client->fd);
}

void on_disconnected(client_info_t *client, void *user_data) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client->addr.sin_addr, ip, sizeof(ip));

    printf("❌ [EVENT] Client disconnected: %s:%d (fd=%d)\n",
           ip, ntohs(client->addr.sin_port), client->fd);
}

void on_received(client_info_t *client, const char *data, size_t len, void *user_data) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client->addr.sin_addr, ip, sizeof(ip));

    // 데이터를 문자열로 변환
    char buffer[4096];
    size_t copy_len = len < sizeof(buffer) - 1 ? len : sizeof(buffer) - 1;
    memcpy(buffer, data, copy_len);
    buffer[copy_len] = '\0';

    printf("📨 [EVENT] Received from %s:%d (%zu bytes): %s",
           ip, ntohs(client->addr.sin_port), len, buffer);

    // "quit" 명령 처리
    if (strncmp(buffer, "quit", 4) == 0) {
        printf("   → Client requested disconnect\n");
        socket_server_disconnect(client);
        return;
    }

    // Echo 응답
    char response[4096];
    int response_len = snprintf(response, sizeof(response), "Echo: %s", buffer);

    if (socket_server_send(client, response, response_len) == 0) {
        printf("   → Sent echo response (%d bytes)\n", response_len);
    } else {
        printf("   → Failed to send response\n");
    }
}

// === 메인 함수 ===

int main(int argc, char *argv[]) {
    int port = 8080;

    if (argc > 1) {
        port = atoi(argv[1]);
        if (port < 1 || port > 65535) {
            fprintf(stderr, "Invalid port number\n");
            return 1;
        }
    }

    // 시그널 핸들러 등록
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // 5가지 콜백 설정
    server_callbacks_t callbacks = {
        .on_start = on_start,
        .on_stop = on_stop,
        .on_connection = on_connection,
        .on_disconnected = on_disconnected,
        .on_received = on_received
    };

    // 서버 생성
    g_server = socket_server_create("0.0.0.0", port, &callbacks, NULL);
    if (!g_server) {
        fprintf(stderr, "Failed to create server\n");
        return 1;
    }

    printf("\n=== Echo Server ===\n");
    printf("Listening on port %d\n", port);
    printf("Press Ctrl+C to stop\n\n");

    // 서버 시작 (블로킹)
    socket_server_start(g_server);

    // 정리
    socket_server_destroy(g_server);

    return 0;
}
