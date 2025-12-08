//
// Created by rnasterwoo on 2025. 12. 2..
//

#ifndef QUEKKA_H
#define QUEKKA_H

#include <stdio.h>

#define BUFFER_SIZE 1024
#define BUFFER_DATA_LEN (BUFFER_SIZE -128 -4 -4)

typedef struct {
    char topic[128];
    uint32_t size;
    uint32_t seq;
    char data[BUFFER_DATA_LEN];
} send_buffer_s;

typedef struct {
    const char *payload;
    size_t size;
} message_t;

typedef struct {
    const char name[64];
    message_t message;
} topic_t;

typedef struct quekka_s quekka_t;
struct quekka_s {
    const char *ip;
    int port;
    int sockfd;
    int (*publish)(const quekka_t *self, const char *topic, const char *payload, size_t size);
};

static quekka_t *g_quekka = NULL;

// 글로벌 인스턴스 리턴 함수
// publisher config 에서 싱글턴으로 생성해야 하며, 요청하면 인스턴스 반환해줌
quekka_t *get_instance(void);

// 메시지 발행 함수
int publish(const quekka_t *self, const char *topic, const char *payload, size_t size);

#endif //QUEKKA_H