//
// Created by jaeseong on 2025. 12. 10..
//

#ifndef QUEKKA_QUEKKA_PRODUCER_H
#define QUEKKA_QUEKKA_PRODUCER_H

#include <stddef.h>
#include "Quekka_config.h"

typedef struct Quekka_pub {
    int _fd;
} Quekka_producer;

int Quekka_producer_init(Quekka_config config);

/**
 *
 * @param topic
 * @param payload
 * @param size
 * @return
 */
int Quekka_producer_pub(const char *topic, const char *payload, size_t size);

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