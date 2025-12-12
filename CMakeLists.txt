cmake_minimum_required(VERSION 3.15)
project(quekka VERSION 1.0 LANGUAGES C CXX)

# C/C++ 표준 설정
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 빌드 타입 설정 (기본값: Debug)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 컴파일 옵션
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# 출력 디렉토리 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# pthread 라이브러리 찾기
find_package(Threads REQUIRED)

# =============================================================================
# 공통 Include 경로
# =============================================================================
include_directories(${CMAKE_SOURCE_DIR}/include)

# =============================================================================
# Common Library (공통 라이브러리)
# =============================================================================
add_library(quekka_common STATIC
        libsrc/quekka/Quekka_log.c
        include/quekka/Quekka_log.h
)

target_link_libraries(quekka_common PUBLIC
    Threads::Threads
)

# =============================================================================
# Server Library (서버 라이브러리)
# =============================================================================
# Note: epoll_handler, socket_server, example_echo는 Linux 전용 (epoll 사용)
if(UNIX AND NOT APPLE)
    # Linux: epoll 사용 가능
    add_library(quekka_server_lib STATIC
            libsrc/core/tcp_socket.c
            libsrc/core/epoll_handler.c
            libsrc/core/client_manager.c
            libsrc/core/socket_server.c
            libsrc/core/comm.c
            libsrc/core/commManager.c
        # Public headers
            include/core/socket_server.h
        # Internal headers
            include/core/tcp_socket.h
            include/core/epoll_handler.h
            include/core/client_manager.h
            include/core/comm.h
            include/core/commManager.h
    )
else()
    # macOS/Other: epoll 없이 기본 기능만
    add_library(quekka_server_lib STATIC
            libsrc/core/tcp_socket.c
            libsrc/core/client_manager.c
            libsrc/core/comm.c
            libsrc/core/commManager.c
        # Public headers
            include/core/socket_server.h
        # Internal headers
            include/core/tcp_socket.h
            include/core/client_manager.h
            include/core/comm.h
            include/core/commManager.h
    )
endif()

target_link_libraries(quekka_server_lib PUBLIC
    quekka_common
)

# =============================================================================
# Producer Client Library (프로듀서 클라이언트 라이브러리)
# =============================================================================
# 카프카의 kafka-clients 처럼 사용자가 자신의 프로젝트에서 링크하여 사용
#
# 사용법 (외부 프로젝트에서):
#   find_package(quekka REQUIRED)
#   target_link_libraries(my_app PRIVATE quekka_producer)
#
# 또는 직접 링크:
#   gcc my_app.c -I/path/to/quekka/include -L/path/to/quekka/lib -lquekka_producer
# =============================================================================

# =============================================================================
# Main Executable (quekka)
# =============================================================================
add_executable(quekka
        src/server/quekka.cpp
        libsrc/core/libsocket.c
        libsrc/core/libfile.c
)

target_link_libraries(quekka PRIVATE
    quekka_server_lib
)

# =============================================================================
# 설치 규칙
# =============================================================================
install(TARGETS quekka
    RUNTIME DESTINATION bin
)

install(TARGETS quekka_common quekka_server_lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/quekka DESTINATION include)

# =============================================================================
# 빌드 정보 출력
# =============================================================================
message(STATUS "")
message(STATUS "====================================")
message(STATUS "  Quekka Build Configuration")
message(STATUS "====================================")
message(STATUS "  Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  C compiler:        ${CMAKE_C_COMPILER}")
message(STATUS "  CXX compiler:      ${CMAKE_CXX_COMPILER}")
message(STATUS "  Output directory:  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "====================================")
message(STATUS "")
add_subdirectory(src/unittest)
