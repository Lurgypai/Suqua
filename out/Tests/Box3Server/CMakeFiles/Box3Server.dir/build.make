# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/isaiah/Documents/Programming/C++/Projects/Stabby

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/isaiah/Documents/Programming/C++/Projects/Stabby/out

# Include any dependencies generated for this target.
include Tests/Box3Server/CMakeFiles/Box3Server.dir/depend.make

# Include the progress variables for this target.
include Tests/Box3Server/CMakeFiles/Box3Server.dir/progress.make

# Include the compile flags for this target's objects.
include Tests/Box3Server/CMakeFiles/Box3Server.dir/flags.make

Tests/Box3Server/CMakeFiles/Box3Server.dir/Box3Server.cpp.o: Tests/Box3Server/CMakeFiles/Box3Server.dir/flags.make
Tests/Box3Server/CMakeFiles/Box3Server.dir/Box3Server.cpp.o: ../Tests/Box3Server/Box3Server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/isaiah/Documents/Programming/C++/Projects/Stabby/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Tests/Box3Server/CMakeFiles/Box3Server.dir/Box3Server.cpp.o"
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/Box3Server && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Box3Server.dir/Box3Server.cpp.o -c /home/isaiah/Documents/Programming/C++/Projects/Stabby/Tests/Box3Server/Box3Server.cpp

Tests/Box3Server/CMakeFiles/Box3Server.dir/Box3Server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Box3Server.dir/Box3Server.cpp.i"
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/Box3Server && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/isaiah/Documents/Programming/C++/Projects/Stabby/Tests/Box3Server/Box3Server.cpp > CMakeFiles/Box3Server.dir/Box3Server.cpp.i

Tests/Box3Server/CMakeFiles/Box3Server.dir/Box3Server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Box3Server.dir/Box3Server.cpp.s"
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/Box3Server && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/isaiah/Documents/Programming/C++/Projects/Stabby/Tests/Box3Server/Box3Server.cpp -o CMakeFiles/Box3Server.dir/Box3Server.cpp.s

# Object files for target Box3Server
Box3Server_OBJECTS = \
"CMakeFiles/Box3Server.dir/Box3Server.cpp.o"

# External object files for target Box3Server
Box3Server_EXTERNAL_OBJECTS =

Tests/Box3Server/Box3Server: Tests/Box3Server/CMakeFiles/Box3Server.dir/Box3Server.cpp.o
Tests/Box3Server/Box3Server: Tests/Box3Server/CMakeFiles/Box3Server.dir/build.make
Tests/Box3Server/Box3Server: Suqua/libSuqua.a
Tests/Box3Server/Box3Server: Tests/Box3Core/libBox3Core.a
Tests/Box3Server/Box3Server: Suqua/libSuqua.a
Tests/Box3Server/Box3Server: enet-master/libenet.a
Tests/Box3Server/Box3Server: Tests/Box3Server/CMakeFiles/Box3Server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/isaiah/Documents/Programming/C++/Projects/Stabby/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Box3Server"
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/Box3Server && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Box3Server.dir/link.txt --verbose=$(VERBOSE)
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/Box3Server && /usr/bin/cmake -E copy_directory /home/isaiah/Documents/Programming/C++/Projects/Stabby/Tests/Box3Server/rsrc/ /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/Box3Server

# Rule to build all files generated by this target.
Tests/Box3Server/CMakeFiles/Box3Server.dir/build: Tests/Box3Server/Box3Server

.PHONY : Tests/Box3Server/CMakeFiles/Box3Server.dir/build

Tests/Box3Server/CMakeFiles/Box3Server.dir/clean:
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/Box3Server && $(CMAKE_COMMAND) -P CMakeFiles/Box3Server.dir/cmake_clean.cmake
.PHONY : Tests/Box3Server/CMakeFiles/Box3Server.dir/clean

Tests/Box3Server/CMakeFiles/Box3Server.dir/depend:
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/isaiah/Documents/Programming/C++/Projects/Stabby /home/isaiah/Documents/Programming/C++/Projects/Stabby/Tests/Box3Server /home/isaiah/Documents/Programming/C++/Projects/Stabby/out /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/Box3Server /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/Box3Server/CMakeFiles/Box3Server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Tests/Box3Server/CMakeFiles/Box3Server.dir/depend

