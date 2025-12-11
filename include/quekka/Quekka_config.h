//
// Created by jaeseong on 2025. 12. 10..
//

#ifndef QUEKKA_QUEKKA_CONFIG_H
#define QUEKKA_QUEKKA_CONFIG_H

typedef struct _Quekka_config {
    char _ip[32];
    char _port[32];
} Quekka_config;

/**
 * address "localhost:9999"
 * @return
 */
int Quekka_config_set_address(const char *address);
int Quekka_config_set_ip(const char *ip);
int Quekka_config_set_port(const char *port);

int Quekka_config_get_ip(char *ip);
int Quekka_config_get_port(char *port);

#endif //QUEKKA_QUEKKA_CONFIG_H