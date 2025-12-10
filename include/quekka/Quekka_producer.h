//
// Created by jaeseong on 2025. 12. 10..
//

#ifndef QUEKKA_QUEKKA_PRODUCER_H
#define QUEKKA_QUEKKA_PRODUCER_H


struct Quekka_producer {
    int _fd;
}typedef Quekka_producer;

int Quekka_producer_init();

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