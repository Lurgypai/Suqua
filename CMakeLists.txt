# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)


project ("Suqua")

# Include sub-projects.
add_subdirectory ("Suqua")
add_subdirectory ("enet-master")

# add_subdirectory("RuinsGame")
add_subdirectory("Ishparsh")
