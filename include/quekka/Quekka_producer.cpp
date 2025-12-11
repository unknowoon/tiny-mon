//
// Created by jaeseong on 2025. 12. 10..
//

#include "Quekka_producer.h"
#include "Quekka_config.h"

#include <cstdlib>
#include <cstring>
#include <sys/socket.h>


static Quekka_producer _producer;

int Quekka_producer_init(Quekka_config config) {
    memset(&_producer, 0, sizeof(_producer));

    if (config.status)
        return -1;
    else {
        _producer._fd = Quekka_config_get_fd();
    }
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



