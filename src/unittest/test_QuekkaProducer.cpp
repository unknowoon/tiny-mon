//
// Created by b532882 on 12/1/25.
//
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <cstring>

extern "C" {
#include "quekka/Quekka_producer.h"
#include "quekka/Quekka_config.h"
}

// ------------------ 테스트 예시 ------------------
// 실행 방법:
//   단위 테스트만: ./src_unittest --gtest_filter=*UnitTest*
//   통합 테스트만: ./src_unittest --gtest_filter=*IntegrationTest*

using namespace std;

// ==============================================================
// QuekkaConfigUnitTest - Config 관련 단위 테스트 (서버 불필요)
// ==============================================================

// 컨피그 초기화 테스트
TEST(QuekkaConfigUnitTest, ConfigInit) {
    Quekka_config config;
    int result = Quekka_config_init(&config);
    EXPECT_EQ(result, 0);
}

// 컨피그 주소 설정 테스트
TEST(QuekkaConfigUnitTest, ConfigSetAddress) {
    Quekka_config config;
    Quekka_config_init(&config);

    int result = Quekka_config_set_address(&config, "127.0.0.1:9999");
    EXPECT_EQ(result, 0);

    char ip[INET_ADDRSTRLEN];
    Quekka_config_get_ip(&config, ip);
    EXPECT_STREQ(ip, "127.0.0.1");
}

// ==============================================================
// QuekkaProducerUnitTest - Producer 관련 단위 테스트 (서버 불필요)
// ==============================================================

// 헤더 크기 검증 테스트
TEST(QuekkaProducerUnitTest, HeaderSizeValidation) {
    EXPECT_EQ(QUEKKA_TOPIC_MAX, 128);
    EXPECT_EQ(QUEKKA_MSG_ID_MAX, 18);
    EXPECT_EQ(QUEKKA_HEADER_SIZE, 153);
    EXPECT_EQ(QUEKKA_MSG_SIZE, 4096);
    EXPECT_EQ(QUEKKA_PAYLOAD_MAX, QUEKKA_MSG_SIZE - QUEKKA_HEADER_SIZE);
}

// 메시지 플래그 테스트
TEST(QuekkaProducerUnitTest, MessageFlags) {
    EXPECT_EQ(QUEKKA_FLAG_MORE, 0x00);
    EXPECT_EQ(QUEKKA_FLAG_LAST, 0x01);
}

// ==============================================================
// QuekkaProducerIntegrationTest - 통합 테스트 (서버 연결 필요)
// ==============================================================

// 프로듀서 초기화 및 발행 테스트
TEST(QuekkaProducerIntegrationTest, ProducerInitAndPublish) {
    // 컨피그 생성
    Quekka_config config;
    Quekka_config_init(&config);
    Quekka_config_set_address(&config, "192.0.0.2:9999");

    // 프로듀서 생성
    Quekka_producer *producer = Quekka_producer_init(&config);
    ASSERT_NE(producer, nullptr);

    // 메시지 발행
    const char *topic = "test_topic";
    const char *payload = "hello world";
    size_t size = strlen(topic) + strlen(payload);

    int result = producer->Quekka_publish(topic, payload, size);
    EXPECT_EQ(result, 0);
}
