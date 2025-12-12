//
// Created by b532882 on 12/1/25.
//
#include <gtest/gtest.h>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <string>
#include <tuple>
#include <future>
#include <thread>

#include "quekka/Quekka_producer.h"

// ------------------ 테스트 예시 ------------------

using namespace std;

TEST(publisher, usage) {
    string ip = "127.0.0.1";
    string port = "8080";

    // 컨피그 생성
    Quekka_config config;
    Quekka_config_init(&config);
    Quekka_config_set_address(&config, "127.0.0.1:9999");

    // 프로듀서 생성

    int status = quekka.connect();
    if (status == -1) {
        printf ("Can't connect to quekka server\n");
    }

    string topic = "hello";
    int CanConn? = publisher.greet (quekka, topic);
    if (CanConn) {
        cout << "success" << endl;
    }

    while (1) {
        sleep(1);
        publisher.publish("it's me, publisher");
    }
}