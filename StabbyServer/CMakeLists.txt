cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE source_files
	"*.cpp"
)

file(GLOB_RECURSE header_files
	"*.h"
)

add_executable(StabbyServer ${source_files} ${header_files})

target_include_directories(StabbyServer PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Suqua/header"
	"${CMAKE_CURRENT_SOURCE_DIR}/../StabbyCore/include"
)

target_link_libraries(StabbyServer ${SDL2_LIBRARIES} StabbyCore enet)

add_custom_command(
    TARGET StabbyServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/rsrc/
        $<TARGET_FILE_DIR:StabbyServer>
)