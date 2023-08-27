# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)


project ("Suqua")

# Include sub-projects.
add_subdirectory ("Suqua")
add_subdirectory ("enet-master")

add_subdirectory("RuinsGame")

if (MSVC)
set_target_properties("Suqua" "RuinsGame" PROPERTIES LINK_FLAGS "/PROFILE")
set_target_properties("Suqua" "RuinsGame" PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
