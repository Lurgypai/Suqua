cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE source_files
	"*.cpp"
)

file(GLOB_RECURSE header_files
	"*.h"
)

add_library(StabbyCore ${source_files} ${header_files})

target_include_directories(StabbyCore PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Suqua/header"
)

target_link_libraries(StabbyCore Suqua)