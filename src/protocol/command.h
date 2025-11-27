/**
 * TinyMQ - Command Definitions
 *
 * 프로토콜 명령어 및 상수 정의
 */

#ifndef TINYMQ_COMMAND_H
#define TINYMQ_COMMAND_H

#include <stdint.h>

/* ============================================================
 * 명령어 타입 정의
 * ============================================================ */

typedef enum {
    /* Producer 명령어 */
    CMD_PUBLISH = 1,        // 메시지 발행

    /* Consumer 명령어 */
    CMD_SUBSCRIBE = 10,     // 토픽 구독
    CMD_POLL = 11,          // 메시지 폴링
    CMD_COMMIT = 12,        // Offset 커밋

    /* 관리 명령어 */
    CMD_CREATE = 20,        // 토픽 생성
    CMD_DELETE = 21,        // 토픽 삭제
    CMD_TOPICS = 22,        // 토픽 목록
    CMD_INFO = 23,          // 토픽 정보
    CMD_STATS = 24,         // 서버 상태

    /* 연결 명령어 */
    CMD_QUIT = 99,          // 연결 종료

    /* 에러 */
    CMD_UNKNOWN = -1        // 알 수 없는 명령
} command_type_t;

/* ============================================================
 * 응답 코드 정의
 * ============================================================ */

typedef enum {
    RESP_OK = 0,
    RESP_MESSAGES,          // 메시지 응답
    RESP_EMPTY,             // 메시지 없음

    /* 에러 코드 */
    RESP_ERR_UNKNOWN_CMD = 100,     // 알 수 없는 명령어
    RESP_ERR_TOPIC_NOT_FOUND,       // 토픽 없음
    RESP_ERR_TOPIC_EXISTS,          // 토픽 이미 존재
    RESP_ERR_INVALID_OFFSET,        // 잘못된 offset
    RESP_ERR_STORAGE_FULL,          // 저장 공간 부족
    RESP_ERR_INVALID_FORMAT,        // 프로토콜 형식 오류
    RESP_ERR_GROUP_NOT_FOUND,       // Consumer Group 없음
    RESP_ERR_INTERNAL               // 내부 오류
} response_code_t;

/* ============================================================
 * 프로토콜 상수
 * ============================================================ */

#define TINYMQ_DEFAULT_PORT     9999
#define TINYMQ_MAX_TOPIC_NAME   64
#define TINYMQ_MAX_GROUP_ID     64
#define TINYMQ_MAX_PAYLOAD_SIZE (10 * 1024 * 1024)  // 10MB max message
#define TINYMQ_CRLF             "\r\n"
#define TINYMQ_CRLF_LEN         2

/* 명령어 문자열 */
#define CMD_STR_PUBLISH     "PUBLISH"
#define CMD_STR_SUBSCRIBE   "SUBSCRIBE"
#define CMD_STR_POLL        "POLL"
#define CMD_STR_COMMIT      "COMMIT"
#define CMD_STR_CREATE      "CREATE"
#define CMD_STR_DELETE      "DELETE"
#define CMD_STR_TOPICS      "TOPICS"
#define CMD_STR_INFO        "INFO"
#define CMD_STR_STATS       "STATS"
#define CMD_STR_QUIT        "QUIT"

/* ============================================================
 * 유틸리티 함수
 * ============================================================ */

/**
 * 명령어 문자열을 command_type_t로 변환
 * @param str 명령어 문자열
 * @return 명령어 타입 (실패 시 CMD_UNKNOWN)
 */
static inline command_type_t command_from_string(const char* str) {
    if (!str) return CMD_UNKNOWN;

    if (strcmp(str, CMD_STR_PUBLISH) == 0)   return CMD_PUBLISH;
    if (strcmp(str, CMD_STR_SUBSCRIBE) == 0) return CMD_SUBSCRIBE;
    if (strcmp(str, CMD_STR_POLL) == 0)      return CMD_POLL;
    if (strcmp(str, CMD_STR_COMMIT) == 0)    return CMD_COMMIT;
    if (strcmp(str, CMD_STR_CREATE) == 0)    return CMD_CREATE;
    if (strcmp(str, CMD_STR_DELETE) == 0)    return CMD_DELETE;
    if (strcmp(str, CMD_STR_TOPICS) == 0)    return CMD_TOPICS;
    if (strcmp(str, CMD_STR_INFO) == 0)      return CMD_INFO;
    if (strcmp(str, CMD_STR_STATS) == 0)     return CMD_STATS;
    if (strcmp(str, CMD_STR_QUIT) == 0)      return CMD_QUIT;

    return CMD_UNKNOWN;
}

/**
 * command_type_t를 문자열로 변환
 * @param cmd 명령어 타입
 * @return 명령어 문자열
 */
static inline const char* command_to_string(command_type_t cmd) {
    switch (cmd) {
        case CMD_PUBLISH:   return CMD_STR_PUBLISH;
        case CMD_SUBSCRIBE: return CMD_STR_SUBSCRIBE;
        case CMD_POLL:      return CMD_STR_POLL;
        case CMD_COMMIT:    return CMD_STR_COMMIT;
        case CMD_CREATE:    return CMD_STR_CREATE;
        case CMD_DELETE:    return CMD_STR_DELETE;
        case CMD_TOPICS:    return CMD_STR_TOPICS;
        case CMD_INFO:      return CMD_STR_INFO;
        case CMD_STATS:     return CMD_STR_STATS;
        case CMD_QUIT:      return CMD_STR_QUIT;
        default:            return "UNKNOWN";
    }
}

/**
 * 에러 코드를 문자열로 변환
 * @param code 응답 코드
 * @return 에러 메시지 문자열
 */
static inline const char* response_code_to_string(response_code_t code) {
    switch (code) {
        case RESP_OK:                   return "OK";
        case RESP_MESSAGES:             return "MESSAGES";
        case RESP_EMPTY:                return "EMPTY";
        case RESP_ERR_UNKNOWN_CMD:      return "ERR_UNKNOWN_CMD";
        case RESP_ERR_TOPIC_NOT_FOUND:  return "ERR_TOPIC_NOT_FOUND";
        case RESP_ERR_TOPIC_EXISTS:     return "ERR_TOPIC_EXISTS";
        case RESP_ERR_INVALID_OFFSET:   return "ERR_INVALID_OFFSET";
        case RESP_ERR_STORAGE_FULL:     return "ERR_STORAGE_FULL";
        case RESP_ERR_INVALID_FORMAT:   return "ERR_INVALID_FORMAT";
        case RESP_ERR_GROUP_NOT_FOUND:  return "ERR_GROUP_NOT_FOUND";
        case RESP_ERR_INTERNAL:         return "ERR_INTERNAL";
        default:                        return "ERR_UNKNOWN";
    }
}

#endif /* TINYMQ_COMMAND_H */
