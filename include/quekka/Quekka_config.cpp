//
// Created by jaeseong on 2025. 12. 10..
//

#include "Quekka_config.h"

#include <cstring>

static struct _Quekka_config config;

int Quekka_config_init(const char *address)
{
    strncpy( config._ip, address, sizeof(config._ip) );
    strncpy( config._port, address, sizeof(config._ip) );
}
