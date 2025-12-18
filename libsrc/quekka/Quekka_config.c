//
// Created by jaeseong on 2025. 12. 10..
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quekka/Quekka_config.h"

#include <netinet/in.h>

int Quekka_config_init(Quekka_config *config) {
    memset(config, 0, sizeof(Quekka_config));
    return 0;
}

int Quekka_config_set_address(Quekka_config *config, const char *address) {
    char ip[64] = {0};
    char port[16] = {0};
    int ret = sscanf(address, "%63[^:]:%15s", ip, port);
    memcpy(config->_ip, ip, strlen(ip));
    config->_port = atoi(port);
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
    config->_port = htons(atoi(port));
    return 0;
}

int Quekka_config_get_ip(const Quekka_config *config, char *ip) {
    if (strlen(config->_ip) > 0) {
        memcpy(ip, config->_ip, sizeof(config->_ip));
        return 0;
    }
    return -1;
}

uint16_t Quekka_config_get_port(const Quekka_config *config) {
    return config->_port;
}
