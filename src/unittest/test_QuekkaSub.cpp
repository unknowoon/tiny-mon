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
// ------------------ 테스트 예시 ------------------

using namespace std;

TEST(subscriber, usage) {
    Subscriber subscriber;

    string ip = "127.0.0.1";
    string port = "8080";

    Quekka quekka{ip, port};
    int status = quekka.connect();
    if (status == -1) {
        printf ("Can't connect to quekka server\n");
    }

    string topic = "hello";
    int CanConn? = subscriber.greet (quekka, topic);
    if (CanConn) {
        cout << "success" << endl;
    }

    while (1) {
        // 관심 있는 topic으로 부터 메시지를 수신.
        subscriber.waiting();
    }
}
