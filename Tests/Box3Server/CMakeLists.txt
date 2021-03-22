cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE source_files
	"*.cpp"
)

file(GLOB_RECURSE header_files
	"*.h"
)

add_executable(Box3Server ${source_files} ${header_files})

target_include_directories(Box3 PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Suqua/header"
)

target_link_libraries(Box3Server ${SDL2_LIBRARIES} Suqua Box3Core)

add_custom_command(
    TARGET Box3Server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/rsrc/
        $<TARGET_FILE_DIR:Box3Server>
)