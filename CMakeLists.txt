# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.5)


project ("Suqua")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Include sub-projects.
add_subdirectory ("Suqua")
add_subdirectory ("enet-master")

# add_subdirectory("Demo/Platformer")
add_subdirectory("Demo/TextureRender")

# add_subdirectory("Game/RuinsGame")

add_subdirectory("Game/Shooty2Client")
add_subdirectory("Game/Shooty2Core")
add_subdirectory("Game/Shooty2Server")

add_subdirectory("Test")
