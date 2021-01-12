#
# Find SDL_MIXER
#

# Additional modules
include(FindPackageHandleStandardArgs)

if(UNIX)
# Find include files
find_path(
  SDL_MIXER_INCLUDE_DIR
  PATHS
  /usr/include
  /usr/local/include
  /sw/include
  /opt/local/include
  ${SDL_MIXER_ROOT_DIR}/include
  DOC "The directory where SDL_mixer.h resides")

find_library(SDL_MIXER_LIBRARY
  NAMES SDL2_mixer
  HINTS
    ENV SDLMIXERDIR
    ENV SDLDIR
  PATH_SUFFIXES lib
)

# Handle REQUIRD argument, define *_FOUND variable
#find_package_handle_standard_args(SDL_MIXER_INCLUDE_DIR)

# Hide some variables
mark_as_advanced(SDL_MIXER_INCLUDE_DIR)
elseif(WIN32)
set(SDL2_MIXER_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../windows-sdl2_mixer/include")

set(SDL2_MIXER_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/../windows-sdl2_mixer/lib/SDL_mixer.lib")

string(STRIP "${SDL2_MIXER_LIBRARIES}" SDL2_MIXER_LIBRARIES)
endif()