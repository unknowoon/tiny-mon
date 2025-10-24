cmake_minimum_required(VERSION 3.10)
project(tiny-mon VERSION 1.0 LANGUAGES C)

# C 표준 설정
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 빌드 타입 설정 (기본값: Debug)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 컴파일 옵션
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

# 출력 디렉토리 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libsrc)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libsrc)

# pthread 라이브러리 찾기
find_package(Threads REQUIRED)

# Logger 라이브러리 빌드
add_library(logger STATIC
    libsrc/libLogger.c
    libsrc/libLogger.h
)

target_include_directories(logger PUBLIC
    ${CMAKE_SOURCE_DIR}/libsrc
)

target_link_libraries(logger PRIVATE
    Threads::Threads
)

# Socket Server 라이브러리 빌드
add_library(socket_server STATIC
    src/socket_server.c
    src/socket_server.h
)

target_include_directories(socket_server PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# CommManager 실행 파일 빌드
add_executable(commManager
    src/commManager.c
    src/comm.h
)

target_include_directories(commManager PRIVATE
    ${CMAKE_SOURCE_DIR}/libsrc
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(commManager PRIVATE
    logger
    Threads::Threads
)

# Comm 실행 파일 빌드
add_executable(comm
    src/comm.c
    src/comm.h
)

target_include_directories(comm PRIVATE
    ${CMAKE_SOURCE_DIR}/libsrc
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(comm PRIVATE
    logger
    Threads::Threads
)

# 설치 규칙
install(TARGETS commManager comm logger socket_server
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION libsrc
    ARCHIVE DESTINATION libsrc
)

# 빌드 정보 출력
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C flags: ${CMAKE_C_FLAGS}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
