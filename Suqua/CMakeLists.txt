
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

file(GLOB source_files
	"src/*.cpp"
	"header/*.h"
)

add_library(Suqua ${source_files} "src/glad.c" )

target_include_directories(Suqua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
target_include_directories(Suqua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(Suqua ${CMAKE_DL_LIBS} enet)

if(UNIX)
	target_link_libraries(Suqua stdc++fs)
endif()