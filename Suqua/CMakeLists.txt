cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

if(UNIX)
    find_package(SDL2 REQUIRED HINTS /usr/lib/x86_64-linux-gnu/)
elseif(WIN32)
    set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../windows-sdl2/include)
    set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../windows-sdl2/lib/x64/SDL2.lib;${CMAKE_CURRENT_SOURCE_DIR}/../windows-sdl2/lib/x64/SDL2main.lib")
endif()

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
