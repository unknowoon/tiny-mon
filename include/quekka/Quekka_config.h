//
// Created by jaeseong on 2025. 12. 10..
//

#ifndef QUEKKA_QUEKKA_CONFIG_H
#define QUEKKA_QUEKKA_CONFIG_H
#include <cstdint>
#include <netinet/in.h>

typedef struct Quekka_config {
    char _ip[INET_ADDRSTRLEN];
    uint16_t _port;
} Quekka_config;

/**
 * address "localhost:9999"
 * @return
 */
int Quekka_config_init(Quekka_config *config);
int Quekka_config_set_address(Quekka_config *config, const char *address);
int Quekka_config_set_ip(Quekka_config *config, const char *ip);
int Quekka_config_set_port(Quekka_config *config, const char *port);

int Quekka_config_get_ip(const Quekka_config *config, char *ip);
int Quekka_config_get_port(const Quekka_config *config, char *port);

#endif //QUEKKA_QUEKKA_CONFIG_H