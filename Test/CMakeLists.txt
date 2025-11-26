cmake_minimum_required(VERSION 3.20)

project(suqua_test)

add_executable(test-pool "./test_pool.cpp")
add_executable(networking-benchmark "./networking_benchmark.cpp")
add_executable(test-bytestream "./bytestream_test.cpp")

target_link_libraries(test-pool Suqua)
target_link_libraries(networking-benchmark Suqua)
target_link_libraries(test-bytestream Suqua)
target_include_directories(test-pool PRIVATE ${CMAKE_SOURCE_DIR}/Suqua/header)
target_include_directories(networking-benchmark PRIVATE ${CMAKE_SOURCE_DIR}/Suqua/header)
target_include_directories(test-bytestream PRIVATE ${CMAKE_SOURCE_DIR}/Suqua/header)
