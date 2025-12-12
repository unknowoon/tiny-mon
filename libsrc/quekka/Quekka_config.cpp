//
// Created by jaeseong on 2025. 12. 10..
//

#include <cstdio>
#include <cstring>

#include "quekka/Quekka_config.h"

// static struct _Quekka_config config;

int Quekka_config_init(Quekka_config *config) {
    memset(config, 0, sizeof(Quekka_config));
    return 0;
}

int Quekka_config_set_address(Quekka_config *config, const char *address) {
    int ret = sscanf(address, "%[^:]:%s", config->_ip, config->_port);
    if (ret != 2)
        return -1;
    return 0;
}

int Quekka_config_set_ip(Quekka_config *config, const char *ip) {
    memset(config->_ip, 0x00, sizeof(config->_ip));
    strncpy(config->_ip, ip, sizeof(config->_ip));
    return 0;
}

int Quekka_config_set_port(Quekka_config *config, const char *port) {
    memset(config->_port, 0x00, sizeof(config->_port));
    strncpy(config->_port, port, sizeof(config->_port));
    return 0;
}

int Quekka_config_get_ip(const Quekka_config *config, char *ip) {
    if (strlen(config->_ip) > 0) {
        memcpy(ip, config->_ip, sizeof(config->_ip));
        return 0;
    }
    return -1;
}

int Quekka_config_get_port(const Quekka_config *config, char *port) {
    if (strlen(config->_port) > 0) {
        memcpy(port, config->_port, sizeof(config->_port));
        return 0;
    }
    return -1;
}
