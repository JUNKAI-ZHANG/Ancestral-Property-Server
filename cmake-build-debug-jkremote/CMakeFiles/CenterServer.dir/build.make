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
include CMakeFiles/CenterServer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CenterServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CenterServer.dir/flags.make

CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o: CMakeFiles/CenterServer.dir/flags.make
CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o: ../src/Source/CenterServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o -c /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/CenterServer.cpp

CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/CenterServer.cpp > CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.i

CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/CenterServer.cpp -o CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.s

CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o.requires:

.PHONY : CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o.requires

CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o.provides: CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o.requires
	$(MAKE) -f CMakeFiles/CenterServer.dir/build.make CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o.provides.build
.PHONY : CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o.provides

CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o.provides.build: CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o


# Object files for target CenterServer
CenterServer_OBJECTS = \
"CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o"

# External object files for target CenterServer
CenterServer_EXTERNAL_OBJECTS = \
"/home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o" \
"/home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o"

CenterServer: CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o
CenterServer: CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o
CenterServer: CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o
CenterServer: CMakeFiles/CenterServer.dir/build.make
CenterServer: lib/libCommonUtilLib.a
CenterServer: lib/libProtoUtilLib.a
CenterServer: CMakeFiles/CenterServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable CenterServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CenterServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CenterServer.dir/build: CenterServer

.PHONY : CMakeFiles/CenterServer.dir/build

CMakeFiles/CenterServer.dir/requires: CMakeFiles/CenterServer.dir/src/Source/CenterServer.cpp.o.requires

.PHONY : CMakeFiles/CenterServer.dir/requires

CMakeFiles/CenterServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CenterServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CenterServer.dir/clean

CMakeFiles/CenterServer.dir/depend:
	cd /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jk/CodeHome/Ancestral-Property-Client-jiubei /home/jk/CodeHome/Ancestral-Property-Client-jiubei /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles/CenterServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CenterServer.dir/depend

