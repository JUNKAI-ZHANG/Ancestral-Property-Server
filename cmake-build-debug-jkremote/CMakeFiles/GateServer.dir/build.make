# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.3

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jk/CodeHome/Ancestral-Property-Client-jiubei

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote

# Include any dependencies generated for this target.
include CMakeFiles/GateServer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/GateServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/GateServer.dir/flags.make

CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o: CMakeFiles/GateServer.dir/flags.make
CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o: ../src/Source/GateServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o -c /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/GateServer.cpp

CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/GateServer.cpp > CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.i

CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/GateServer.cpp -o CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.s

CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o.requires:

.PHONY : CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o.requires

CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o.provides: CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o.requires
	$(MAKE) -f CMakeFiles/GateServer.dir/build.make CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o.provides.build
.PHONY : CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o.provides

CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o.provides.build: CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o


# Object files for target GateServer
GateServer_OBJECTS = \
"CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o"

# External object files for target GateServer
GateServer_EXTERNAL_OBJECTS = \
"/home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o" \
"/home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o"

GateServer: CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o
GateServer: CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o
GateServer: CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o
GateServer: CMakeFiles/GateServer.dir/build.make
GateServer: lib/libCommonUtilLib.a
GateServer: lib/libProtoUtilLib.a
GateServer: CMakeFiles/GateServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable GateServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/GateServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/GateServer.dir/build: GateServer

.PHONY : CMakeFiles/GateServer.dir/build

CMakeFiles/GateServer.dir/requires: CMakeFiles/GateServer.dir/src/Source/GateServer.cpp.o.requires

.PHONY : CMakeFiles/GateServer.dir/requires

CMakeFiles/GateServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/GateServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/GateServer.dir/clean

CMakeFiles/GateServer.dir/depend:
	cd /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jk/CodeHome/Ancestral-Property-Client-jiubei /home/jk/CodeHome/Ancestral-Property-Client-jiubei /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles/GateServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/GateServer.dir/depend
