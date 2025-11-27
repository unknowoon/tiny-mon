/**
 * TinyMQ - Message Format
 *
 * 디스크에 저장되는 메시지 포맷 정의
 */

#ifndef TINYMQ_MESSAGE_H
#define TINYMQ_MESSAGE_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

/* ============================================================
 * 메시지 헤더 (24바이트, 패딩 없음)
 * ============================================================
 *
 * 디스크 레이아웃:
 * +----------+-------------+----------+-----------+
 * | offset   | payload_size| crc32    | timestamp |
 * | 8 bytes  | 4 bytes     | 4 bytes  | 8 bytes   |
 * +----------+-------------+----------+-----------+
 */

typedef struct __attribute__((packed)) {
    uint64_t offset;        // 메시지 순번 (0부터 시작, 토픽 내 유일)
    uint32_t payload_size;  // 페이로드 크기 (바이트)
    uint32_t crc32;         // 페이로드 CRC32 체크섬
    int64_t  timestamp;     // Unix timestamp (milliseconds)
} message_header_t;

#define MESSAGE_HEADER_SIZE sizeof(message_header_t)  // 24 bytes

/* ============================================================
 * 인덱스 엔트리 (16바이트, 패딩 없음)
 * ============================================================
 *
 * 디스크 레이아웃:
 * +----------+----------+
 * | offset   | position |
 * | 8 bytes  | 8 bytes  |
 * +----------+----------+
 */

typedef struct __attribute__((packed)) {
    uint64_t offset;        // 메시지 offset
    uint64_t position;      // .log 파일 내 바이트 위치
} index_entry_t;

#define INDEX_ENTRY_SIZE sizeof(index_entry_t)  // 16 bytes

/* Sparse index: N개 메시지마다 인덱스 엔트리 기록 */
#define INDEX_INTERVAL 100

/* ============================================================
 * 세그먼트 설정
 * ============================================================ */

/* 세그먼트 최대 크기 (이 크기 초과 시 새 세그먼트 생성) */
#define SEGMENT_MAX_SIZE        (100 * 1024 * 1024)  // 100MB

/* 세그먼트 파일 확장자 */
#define SEGMENT_LOG_EXT         ".log"
#define SEGMENT_INDEX_EXT       ".idx"

/* 세그먼트 파일명 포맷 (20자리 offset) */
#define SEGMENT_NAME_FORMAT     "%020" PRIu64

/* ============================================================
 * 메시지 유틸리티 함수
 * ============================================================ */

/**
 * 현재 타임스탬프 반환 (밀리초)
 */
static inline int64_t message_timestamp_now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/**
 * 메시지 헤더 초기화
 * @param header 초기화할 헤더 포인터
 * @param offset 메시지 offset
 * @param payload_size 페이로드 크기
 * @param crc32 페이로드 체크섬
 */
static inline void message_header_init(message_header_t* header,
                                        uint64_t offset,
                                        uint32_t payload_size,
                                        uint32_t crc32) {
    header->offset = offset;
    header->payload_size = payload_size;
    header->crc32 = crc32;
    header->timestamp = message_timestamp_now();
}

/**
 * 전체 메시지 크기 계산 (헤더 + 페이로드)
 */
static inline size_t message_total_size(const message_header_t* header) {
    return MESSAGE_HEADER_SIZE + header->payload_size;
}

/**
 * 세그먼트 파일명 생성
 * @param buffer 출력 버퍼
 * @param buffer_size 버퍼 크기
 * @param base_path 기본 경로 (예: "data/topics/logs")
 * @param base_offset 세그먼트 시작 offset
 * @param extension 확장자 (".log" 또는 ".idx")
 */
static inline int segment_filename(char* buffer,
                                    size_t buffer_size,
                                    const char* base_path,
                                    uint64_t base_offset,
                                    const char* extension) {
    return snprintf(buffer, buffer_size,
                    "%s/%020" PRIu64 "%s",
                    base_path, base_offset, extension);
}

#endif /* TINYMQ_MESSAGE_H */
