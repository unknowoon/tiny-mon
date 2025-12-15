//
// Created by jaeseong on 2025. 12. 10..
//

#ifndef QUEKKA_QUEKKA_PRODUCER_H
#define QUEKKA_QUEKKA_PRODUCER_H

#include "Quekka_config.h"
#include <stdint.h>

#define QUEKKA_MSG_SIZE       4096
#define QUEKKA_TOPIC_MAX      (127 + 1)   // 토픽 최대 127자 + null
#define QUEKKA_MSG_ID_MAX     (17 + 1)    // yyyymmddhhMMsssss + null
#define QUEKKA_HEADER_SIZE    153         // 128 + 18 + 2 + 2 + 2 + 1
#define QUEKKA_PAYLOAD_MAX    (QUEKKA_MSG_SIZE - QUEKKA_HEADER_SIZE)  // 3943

typedef struct Quekka_producer {
    int _fd;
    Quekka_config *_config;
    int (*Quekka_publish)(const char *topic, const char *payload, size_t size);
} Quekka_producer;

#pragma pack(push, 1)
typedef struct Quekka_header {
    char topic[QUEKKA_TOPIC_MAX];       // 128 bytes
    char message_id[QUEKKA_MSG_ID_MAX]; // 18 bytes (yyyymmddhhMMsssss)
    uint16_t topic_len;                 // 2 bytes
    uint16_t payload_len;               // 2 bytes
    uint16_t seq;                       // 2 bytes - 분할 조각 인덱스
    uint8_t flags;                      // 1 byte  - 0x01: 마지막 조각
} Quekka_header;

typedef struct Quekka_message {
    Quekka_header header;
    char payload[QUEKKA_PAYLOAD_MAX];
} Quekka_message;
#pragma pack(pop)

_Static_assert(sizeof(Quekka_message) == QUEKKA_MSG_SIZE, "Quekka_message must be 4096 bytes");

Quekka_producer *Quekka_producer_init(const Quekka_config *config);

/**
 *
 * @param topic
 * @param payload
 * @param size
 * @return
 */
static int Quekka_publish(const char *topic, const char *payload, size_t size);

/**
 *
 * @param topic
 * @param payload
 * @param size
 * @param timeout
 * @return
 */
int Quekka_producer_pubWithTimeout(const char *topic, const char *payload, size_t size, long timeout);

#endif //QUEKKA_QUEKKA_PRODUCER_H