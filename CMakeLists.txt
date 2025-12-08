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
    src/common/libLogger.c
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
        src/server/tcp_socket.c
        src/server/epoll_handler.c
        src/server/client_manager.c
        src/server/socket_server.c
        src/server/comm.c
        src/server/commManager.c
    )
else()
    # macOS/Other: epoll 없이 기본 기능만
    add_library(quekka_server_lib STATIC
        src/server/tcp_socket.c
        src/server/client_manager.c
        src/server/comm.c
        src/server/commManager.c
    )
endif()

target_link_libraries(quekka_server_lib PUBLIC
    quekka_common
)

# =============================================================================
# Main Executable (quekka)
# =============================================================================
add_executable(quekka
    src/server/main.cpp
    src/oopc/quekka.c
)

target_link_libraries(quekka PRIVATE
    quekka_server_lib
)

# =============================================================================
# Echo Server Example (Linux only - requires epoll)
# =============================================================================
if(UNIX AND NOT APPLE)
    add_executable(example_echo
        src/server/example_echo.c
    )

    target_link_libraries(example_echo PRIVATE
        quekka_server_lib
    )
endif()

# =============================================================================
# 설치 규칙
# =============================================================================
install(TARGETS quekka
    RUNTIME DESTINATION bin
)

if(UNIX AND NOT APPLE)
    install(TARGETS example_echo
        RUNTIME DESTINATION bin
    )
endif()

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
