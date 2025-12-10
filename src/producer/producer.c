//
// Created by rnasterwoo on 25. 12. 9..
//

#include "quekka/producer.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//
// 이 소스코드는 AI로 카프카에 기반한 사용법을 추출하였습니다.
// 아래 소스코드를 기준으로 동작하는 프로듀서를 작성할 예정입니다.
//
struct Quekka_config {
    char ip[32];
    char port[32];
}typedef Quekka_config;

struct Quekka_conn{
    int status; //fd status
    int fd    ; // quekka connection fd, socket fd,
}typedef Quekka_conn;

int main() {

#if 0
    // 1. 설정
    Quekka_config config;
    strncpy(config.ip, "127.0.0.1"); // Quekka_config_set_ip
    strncpy(config.port ,"9999");    // Quekka_config_set_port
#else
    // 1. 설정
    Quekka_config_init();
    Quekka_config_set_address("127.0.0.1:999"); // Quekka_config_set_ip
#endif

    // Quekka_conn 구조체변수 초기화
    struct Quekka_conn = Quekka_conn_init{ config };

    //Establish .., to Quekka server
    if ( !Quekka_conn_init(Quekka_conn) ) {
        printf("Quekka_conn_init failed to connect Quekka");
        return -1;
    }

    Quekka_producer producer;
    Quekka_producer_init(&producer, quekka_establisher);

    while (1) {
        sleep(1);
        Quekka_producer_publish(producer, "hi", 2);
    }

#if 0
    // 2. 프로듀서 생성
    quekka_producer_t *producer = quekka_producer_new(config);

    // 3. 연결
    quekka_producer_connect(producer);

    // 4. 메시지 전송
    quekka_producer_send(producer, "my-topic", "Hello Quekka!", 13);

    // 5. 정리
    quekka_producer_close(producer);
    quekka_config_destroy(config);
#endif

    return 0;
}
