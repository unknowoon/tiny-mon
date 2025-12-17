cmake_minimum_required(VERSION 3.15)
project(src_unittest)

set(CMAKE_CXX_STANDARD 20)

if(NOT "${PROJECT}")
    set(PROJECT ${CMAKE_CURRENT_SOURCE_DIR}/../../)
endif()

# FetchContent로 GoogleTest 자동 다운로드
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
# Windows에서 gtest_force_shared_crt 설정
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(${PROJECT}/src)
include_directories(${PROJECT}/include)

# 테스트 파일
set(TEST_FILES
    test_QuekkaMdb.cpp
    test_QuekkaConfig.cpp
    test_QuekkaProducer.cpp
    test_Epoll.cpp
)

# Quekka 구현 파일
set(QUEKKA_SRC
    ${PROJECT}/libsrc/quekka/Quekka_config.c
    ${PROJECT}/libsrc/quekka/Quekka_producer.c
    ${PROJECT}/libsrc/internal/libsocket.c
)

add_executable(src_unittest test_main.cpp ${TEST_FILES} ${QUEKKA_SRC})

target_include_directories(src_unittest PUBLIC
    ${PROJECT}/include
    ${PROJECT}/src
)

target_link_libraries(src_unittest
    gtest
    gtest_main
    pthread
    quekka_common
)
