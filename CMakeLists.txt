# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_BUILD_TYPE Debug)

project ("StabbyOrganized")

# Include sub-projects.
add_subdirectory ("Suqua")
add_subdirectory ("StabbyCore")
add_subdirectory ("Stabby")
add_subdirectory ("StabbyServer")
add_subdirectory ("enet-master")
