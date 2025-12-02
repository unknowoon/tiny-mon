//
// Created by rnasterwoo on 2025. 12. 2..
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quekka.h"

#include <sys/socket.h>

static quekka_t *make_instance(void) {
    quekka_t *quekka = malloc(sizeof(quekka_t));
    if (quekka == NULL) {
       printf("Malloc failed\n");
    }

    memset(quekka, 0, sizeof(quekka_t));
    return quekka;
}

quekka_t *get_instance(void) {
    if (!g_quekka) {
        g_quekka = make_instance();
    }
    return g_quekka;
}

// 메시지 발행 함수
// 데이터 받는 주체가 명확하지 않다면 nc -l 명령으로 더미 대상으로 송신할 수 있음
// 라고 하심
int publish(const quekka_t *self, const char *topic, const char *payload, const size_t size) {
    const size_t total_chunks = (size + BUFFER_DATA_LEN - 1) / BUFFER_DATA_LEN;
    size_t remaining = size;

    for (uint32_t seq = 0; seq < total_chunks; seq++) {
        send_buffer_s buffer;
        memset(&buffer, 0, sizeof(buffer));

        strncpy(buffer.topic, topic, sizeof(buffer.topic) - 1);
        buffer.seq = seq;

        size_t chunk_size = (remaining > BUFFER_DATA_LEN) ? BUFFER_DATA_LEN : remaining;
        buffer.size = chunk_size;

        memcpy(buffer.data, payload + (seq * BUFFER_DATA_LEN), chunk_size);

        send(self->sockfd, &buffer, sizeof(buffer), 0);

        remaining -= chunk_size;
    }

    return EXIT_SUCCESS;
}
