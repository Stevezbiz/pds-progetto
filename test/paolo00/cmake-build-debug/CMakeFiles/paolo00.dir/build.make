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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/paolo00.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/paolo00.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/paolo00.dir/flags.make

CMakeFiles/paolo00.dir/main.cpp.o: CMakeFiles/paolo00.dir/flags.make
CMakeFiles/paolo00.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/paolo00.dir/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/paolo00.dir/main.cpp.o -c /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00/main.cpp

CMakeFiles/paolo00.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/paolo00.dir/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00/main.cpp > CMakeFiles/paolo00.dir/main.cpp.i

CMakeFiles/paolo00.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/paolo00.dir/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00/main.cpp -o CMakeFiles/paolo00.dir/main.cpp.s

# Object files for target paolo00
paolo00_OBJECTS = \
"CMakeFiles/paolo00.dir/main.cpp.o"

# External object files for target paolo00
paolo00_EXTERNAL_OBJECTS =

paolo00: CMakeFiles/paolo00.dir/main.cpp.o
paolo00: CMakeFiles/paolo00.dir/build.make
paolo00: CMakeFiles/paolo00.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable paolo00"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/paolo00.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/paolo00.dir/build: paolo00

.PHONY : CMakeFiles/paolo00.dir/build

CMakeFiles/paolo00.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/paolo00.dir/cmake_clean.cmake
.PHONY : CMakeFiles/paolo00.dir/clean

CMakeFiles/paolo00.dir/depend:
	cd /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00 /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00 /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00/cmake-build-debug /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00/cmake-build-debug /Users/paologastaldi/Documents/politecnico/PDS/pds-progetto/test/paolo00/cmake-build-debug/CMakeFiles/paolo00.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/paolo00.dir/depend

