cmake_minimum_required(VERSION 3.15)
project(quekka VERSION 1.0 LANGUAGES C CXX)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 빌드 타입 설정 (기본값: Debug)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 컴파일 옵션
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# 출력 디렉토리 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# pthread 라이브러리 찾기
find_package(Threads REQUIRED)

# Logger 라이브러리 빌드
add_library(logger STATIC
    libsrc/libLogger.c
)

target_include_directories(logger PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(logger PRIVATE
    Threads::Threads
)

# Socket Server 라이브러리 빌드
add_library(socket_server STATIC
    src/tcp_socket.c
    src/epoll_handler.c
    src/client_manager.c
    src/socket_server.c
)

target_include_directories(socket_server PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(quekka
        src/main.cpp
        src/comm.c
        src/commManager.c
        src/commManager.h
)

target_include_directories(quekka PRIVATE src libsrc)

target_link_libraries(quekka PRIVATE logger)

# Echo Server 예제
add_executable(example_echo
    src/example_echo.c
)

target_include_directories(example_echo PRIVATE src)

target_link_libraries(example_echo PRIVATE socket_server)

# 설치 규칙
install(TARGETS quekka logger
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 빌드 정보 출력
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")