//
// Created by jaeseong on 2025. 12. 10..
//

#ifndef QUEKKA_QUEKKA_CONFIG_H
#define QUEKKA_QUEKKA_CONFIG_H

typedef struct Quekka_config {
    char _ip[32];
    char _port[8];
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