﻿# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)


project ("Suqua")

# Include sub-projects.
add_subdirectory ("Suqua")
add_subdirectory ("enet-master")

add_subdirectory("Demo/Platformer")
add_subdirectory("Demo/TextureRender")

add_subdirectory("Game/RuinsGame")

add_subdirectory("Game/Shooty2Client")
add_subdirectory("Game/Shooty2Core")
add_subdirectory("Game/Shooty2Server")

# if (MSVC)
    # set_target_properties("Suqua" "Shooty2Core" PROPERTIES LINK_FLAGS "/PROFILE")
    # set_target_properties("Suqua" "Shooty2Client" PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# endif()
