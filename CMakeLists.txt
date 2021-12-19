# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)


project ("Suqua")

# Include sub-projects.
add_subdirectory ("Suqua")
add_subdirectory ("enet-master")
add_subdirectory ("Tests/HRTag")
add_subdirectory ("Tests/CharBuffer")
add_subdirectory ("Tests/NetworkDataComponent")
add_subdirectory ("Tests/ByteStream")
add_subdirectory ("Tests/ByteOrder")
add_subdirectory ("Tests/Box3")
add_subdirectory ("Tests/Box3Server")
add_subdirectory ("Tests/Box3Core")
add_subdirectory ("Tests/SyncState")
add_subdirectory ("Tests/SyncSystem")

add_subdirectory("Tests/Stabby2Client")
add_subdirectory("Tests/Stabby2Core")

if (MSVC)
set_target_properties("Suqua" "Box3Core" "Box3" "Box3Server" "Stabby2Core" "Stabby2Client" PROPERTIES LINK_FLAGS "/PROFILE")
set_target_properties("Suqua" "Box3Core" "Box3" "Box3Server" "Stabby2Core" "Stabby2Client" PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()