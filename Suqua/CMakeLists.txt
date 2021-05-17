cmake_minimum_required (VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED HINTS /usr/lib/x86_64-linux-gnu/)

file(GLOB source_files
	"src/*.cpp"
	"header/*.h"
)

add_library(Suqua ${source_files} "src/glad.c" )

target_include_directories(Suqua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
target_include_directories(Suqua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(Suqua PRIVATE ${CMAKE_DL_LIBS} ${SDL2_LIBRARIES} enet)

if(UNIX)
    target_link_libraries(Suqua PRIVATE stdc++fs)
endif()
