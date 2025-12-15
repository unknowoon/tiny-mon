//
// Created by 우지범 on 2025. 12. 10..
//

#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "internal/libsocket.h"
#include "quekka/Quekka_producer.h"

#include <arpa/inet.h>
#include "quekka/Quekka_config.h"
#include "quekka/Quekka_log.h"

static uint32_t errcode = 0;
static Quekka_producer *g_producer = NULL;
/**
 *
 * @return
 */
Quekka_producer *Quekka_producer_init(const Quekka_config *config) {
    // 프로듀서 객체 초기화
    // g_producer 널 체크
    if (g_producer != NULL) {
        return g_producer;
    }

    // 서버와 tcp 소켓연결
    const int socket_fd = socket_init();
    if (!socket_fd) {
        log_error("failed to create socket.");
        snprintf(errcode, sizeof(errcode)-1, "%s", "1111");
        sprintf(errcode, "%.*s", 4, "1111");
    }

    // 접속 서버 정보 세팅
    struct sockaddr_in server_address;
    socket_set_server_address(&server_address, config->_ip, config->_port);

    // 서버 접속
    socket_connect(&server_address, socket_fd);

    // 메모리 초기화
    g_producer = calloc(1, sizeof(Quekka_producer));
    if (!g_producer) {
        strncpy(errcode, "1111", sizeof(errcode));
        return NULL;
    }

    memcpy(g_producer, config, sizeof(Quekka_producer));
    g_producer->_fd = socket_fd;

    return g_producer;
}

static int Quekka_publish(const char *topic, const char *payload, size_t size) {

    if (size < QUEKKA_PAYLOAD_MAX)

    send(g_producer->_fd, payload, size, 0);
    return 0;
}

static Quekka_message _message_assemble(const char *topic, const char *payload) {

}

int Quekka_producer_pubWithTimeout(const char *topic, const char *payload, size_t size, long timeout) {
    (void)topic;    // unused
    (void)timeout;  // unused
    send(g_producer->_fd, payload, size, 0);
    return 0;
}



