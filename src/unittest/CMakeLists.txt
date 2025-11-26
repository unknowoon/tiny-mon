cmake_minimum_required(VERSION 3.15)
project(src_unittest)

set(CMAKE_CXX_STANDARD 20)

if(NOT "${PROJECT}")
    set(PROJECT ${CMAKE_CURRENT_SOURCE_DIR}/../../)
endif()

include_directories(${PROJECT}/src)
link_directories(SYSTEM ${PROJECT}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/googletest/lib)


set(TEST_FILES
        ../mdb.hpp
        test_mdb.cpp
)

add_executable(src_unittest test_main.cpp ${TEST_FILES})

target_include_directories(src_unittest PUBLIC ${PROJECT}/libsrc ${PROJECT}/src)

target_link_libraries(src_unittest gtest gtest_main pthread logger)