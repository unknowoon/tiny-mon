//
// Created by rnasterwoo on 25. 12. 9..
//

#include "quekka/producer.h"

//
// 이 소스코드는 AI로 카프카에 기반한 사용법을 추출하였습니다.
// 아래 소스코드를 기준으로 동작하는 프로듀서를 작성할 예정입니다.
//
int main() {
    // 1. 설정
    quekka_config_t *config = quekka_config_new();
    quekka_config_set(config, "server", "localhost:9999");

    // 2. 프로듀서 생성
    quekka_producer_t *producer = quekka_producer_new(config);

    // 3. 연결
    quekka_producer_connect(producer);

    // 4. 메시지 전송
    quekka_producer_send(producer, "my-topic", "Hello Quekka!", 13);

    // 5. 정리
    quekka_producer_close(producer);
    quekka_config_destroy(config);

    return 0;
}
