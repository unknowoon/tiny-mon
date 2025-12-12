//
// Created by jaeseong on 2025. 12. 10..
//

#include "../../include/quekka/Quekka_pub.h"
#include "../../include/quekka/Quekka_config.h"

#include <cstdlib>
#include <cstring>
#include <sys/socket.h>


static Quekka_producer _producer;

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



