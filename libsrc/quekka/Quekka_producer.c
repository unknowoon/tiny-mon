//
// Created by 우지범 on 2025. 12. 10..
//

#include <string.h>
#include <sys/socket.h>

#include "quekka/Quekka_producer.h"
#include "quekka/Quekka_config.h"

/**
 * 프로듀서 싱글턴
 */
static Quekka_producer *g_producer = NULL;

int Quekka_producer_init(Quekka_config config) {
    memset(&_producer, 0, sizeof(_producer));
    return 0;
}

int Quekka_producer_pub(const char *topic, const char *payload, size_t size)
{
    send(_producer._fd, payload, size, 0);
    return 0;
}

int Quekka_producer_pubWithTimeout(const char *topic, const char *payload, size_t size, long timeout)
{
    (void)topic;    // unused
    (void)timeout;  // unused
    send(_producer._fd, payload, size, 0);
    return 0;
}



