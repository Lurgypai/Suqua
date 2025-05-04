cmake_minimum_required(VERSION 3.20)

project(suqua_test)

add_executable(test-pool "./test_pool.cpp")

target_link_libraries(test-pool Suqua)
target_include_directories(test-pool PRIVATE ${CMAKE_SOURCE_DIR}/Suqua/header)
