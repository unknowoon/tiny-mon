//
// Created by jaeseong on 2025. 12. 10..
//

#ifndef QUEKKA_QUEKKA_CONFIG_H
#define QUEKKA_QUEKKA_CONFIG_H

typedef struct _Quekka_config {
    char _ip[32];
    char _port[32];
    int status;
} Quekka_config;

/**
 * address "localhost:9999"
 * @return
 */
int Quekka_config_init(const char *address);
int Quekka_config_status();
int Quekka_config_get_fd();
int Quekka_config_set_ip(const char *ip);
int Quekka_config_set_port(const char *port);

#endif //QUEKKA_QUEKKA_CONFIG_H