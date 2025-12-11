#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FILENAME_LEN 256

// 로그 레벨 정의
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

// 로거 구조체
typedef struct {
    FILE *file;
    LogLevel level;
    int console_output;
    int file_output;
    char filename[MAX_FILENAME_LEN];
} Logger;

// 전역 로거
extern Logger g_logger;

// 로거 초기화 및 설정
void logger_init(const char *filename, LogLevel level);
void logger_set_level(LogLevel level);
void logger_enable_console(int enable);
void logger_enable_file(int enable);
void logger_close(void);

// 로그 쓰기 함수
void log_write(LogLevel level, const char *file, const char *function, int line, const char *format, ...);

// 매크로를 통한 로그 함수
#define log_debug(...) log_write(LOG_DEBUG, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_info(...) log_write(LOG_INFO, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_warning(...) log_write(LOG_WARNING, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_error(...) log_write(LOG_ERROR, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_write(LOG_FATAL, __FILE__, __func__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* LOGGER_H */