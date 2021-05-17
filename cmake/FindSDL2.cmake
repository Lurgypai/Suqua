
# Additional modules
include(FindPackageHandleStandardArgs)

if(UNIX)
# Find include files
find_path(
  SDL2_INCLUDE_DIR SDL.h
  PATHS
  /usr/include
  /usr/local/include
  /sw/include
  /opt/local/include
  ${SDL2_INCLUDE_DIR}/include
  DOC "The directory where SDL.h resides")

find_library(SDL2_LIBRARY
  NAMES SDL2
  HINTS
    ENV SDL2DIR
  PATH_SUFFIXES lib
)

elseif(WIN32)
set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../windows-sdl2/include")

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
  set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/../windows-sdl2/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/../windows-sdl2/lib/x64/SDL2main.lib")
else ()
    set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/../windows-sdl2/lib/x86/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/../windows-sdl2/lib/x86/SDL2main.lib")
endif ()
endif()
