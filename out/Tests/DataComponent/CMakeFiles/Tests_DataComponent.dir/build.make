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
include Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/depend.make

# Include the progress variables for this target.
include Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/progress.make

# Include the compile flags for this target's objects.
include Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/flags.make

Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.o: Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/flags.make
Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.o: ../Tests/DataComponent/Tests_DataComponent.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/isaiah/Documents/Programming/C++/Projects/Stabby/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.o"
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/DataComponent && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.o -c /home/isaiah/Documents/Programming/C++/Projects/Stabby/Tests/DataComponent/Tests_DataComponent.cpp

Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.i"
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/DataComponent && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/isaiah/Documents/Programming/C++/Projects/Stabby/Tests/DataComponent/Tests_DataComponent.cpp > CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.i

Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.s"
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/DataComponent && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/isaiah/Documents/Programming/C++/Projects/Stabby/Tests/DataComponent/Tests_DataComponent.cpp -o CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.s

# Object files for target Tests_DataComponent
Tests_DataComponent_OBJECTS = \
"CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.o"

# External object files for target Tests_DataComponent
Tests_DataComponent_EXTERNAL_OBJECTS =

Tests/DataComponent/Tests_DataComponent: Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/Tests_DataComponent.cpp.o
Tests/DataComponent/Tests_DataComponent: Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/build.make
Tests/DataComponent/Tests_DataComponent: Suqua/libSuqua.a
Tests/DataComponent/Tests_DataComponent: enet-master/libenet.a
Tests/DataComponent/Tests_DataComponent: Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/isaiah/Documents/Programming/C++/Projects/Stabby/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Tests_DataComponent"
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/DataComponent && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Tests_DataComponent.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/build: Tests/DataComponent/Tests_DataComponent

.PHONY : Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/build

Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/clean:
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/DataComponent && $(CMAKE_COMMAND) -P CMakeFiles/Tests_DataComponent.dir/cmake_clean.cmake
.PHONY : Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/clean

Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/depend:
	cd /home/isaiah/Documents/Programming/C++/Projects/Stabby/out && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/isaiah/Documents/Programming/C++/Projects/Stabby /home/isaiah/Documents/Programming/C++/Projects/Stabby/Tests/DataComponent /home/isaiah/Documents/Programming/C++/Projects/Stabby/out /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/DataComponent /home/isaiah/Documents/Programming/C++/Projects/Stabby/out/Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Tests/DataComponent/CMakeFiles/Tests_DataComponent.dir/depend

