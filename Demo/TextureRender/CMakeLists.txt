cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

if(UNIX)
    find_package(SDL2 REQUIRED HINTS /usr/lib/x86_64-linux-gnu/)
elseif(WIN32)
    set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../windows-sdl2/include)
    set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../windows-sdl2/lib/x64/SDL2.lib;${CMAKE_CURRENT_SOURCE_DIR}/../windows-sdl2/lib/x64/SDL2main.lib")
endif()

file(GLOB_RECURSE source_files
	"*.cpp"
)

file(GLOB_RECURSE header_files
	"*.h"
)

add_executable(TextureRender ${source_files} ${header_files})

target_include_directories(TextureRender PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Suqua/header"
)

target_link_libraries(TextureRender ${SDL2_LIBRARIES} Suqua)

add_custom_command(
    TARGET TextureRender POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/rsrc/
        $<TARGET_FILE_DIR:TextureRender>
)
