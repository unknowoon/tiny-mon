/**
 * TinyMQ - CRC32 Checksum
 *
 * IEEE 802.3 CRC32 구현
 */

#ifndef TINYMQ_CRC32_H
#define TINYMQ_CRC32_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * CRC32 계산
 * @param data 데이터 포인터
 * @param length 데이터 길이
 * @return CRC32 체크섬 값
 */
uint32_t crc32(const void* data, size_t length);

/**
 * CRC32 증분 계산 (스트리밍용)
 * @param crc 이전 CRC 값 (첫 호출 시 0xFFFFFFFF)
 * @param data 데이터 포인터
 * @param length 데이터 길이
 * @return 업데이트된 CRC 값
 */
uint32_t crc32_update(uint32_t crc, const void* data, size_t length);

/**
 * CRC32 최종화 (스트리밍 완료 후 호출)
 * @param crc crc32_update의 마지막 반환값
 * @return 최종 CRC32 값
 */
uint32_t crc32_finalize(uint32_t crc);

#ifdef __cplusplus
}
#endif

#endif /* TINYMQ_CRC32_H */
