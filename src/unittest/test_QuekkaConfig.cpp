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

#include "quekka/Quekka_config.h"

// ------------------ 테스트 예시 ------------------

using namespace std;

TEST(Quekka_config, usage) {
    Quekka_config config;

    Quekka_config_set_address("127.0.0.1:8080");

    char ip[16];
    Quekka_config_get_ip(ip);
    EXPECT_STREQ(ip, "127.0.0.1");

    char port[16];
    Quekka_config_get_port(port);
    EXPECT_STREQ(port, "8080");
}