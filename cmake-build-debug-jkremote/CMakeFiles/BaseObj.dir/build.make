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
include CMakeFiles/BaseObj.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/BaseObj.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/BaseObj.dir/flags.make

CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o: CMakeFiles/BaseObj.dir/flags.make
CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o: ../src/Source/ServerBase.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o -c /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/ServerBase.cpp

CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/ServerBase.cpp > CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.i

CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/ServerBase.cpp -o CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.s

CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o.requires:

.PHONY : CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o.requires

CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o.provides: CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o.requires
	$(MAKE) -f CMakeFiles/BaseObj.dir/build.make CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o.provides.build
.PHONY : CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o.provides

CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o.provides.build: CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o


CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o: CMakeFiles/BaseObj.dir/flags.make
CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o: ../src/Source/FuncServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o -c /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/FuncServer.cpp

CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/FuncServer.cpp > CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.i

CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jk/CodeHome/Ancestral-Property-Client-jiubei/src/Source/FuncServer.cpp -o CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.s

CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o.requires:

.PHONY : CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o.requires

CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o.provides: CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o.requires
	$(MAKE) -f CMakeFiles/BaseObj.dir/build.make CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o.provides.build
.PHONY : CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o.provides

CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o.provides.build: CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o


BaseObj: CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o
BaseObj: CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o
BaseObj: CMakeFiles/BaseObj.dir/build.make

.PHONY : BaseObj

# Rule to build all files generated by this target.
CMakeFiles/BaseObj.dir/build: BaseObj

.PHONY : CMakeFiles/BaseObj.dir/build

CMakeFiles/BaseObj.dir/requires: CMakeFiles/BaseObj.dir/src/Source/ServerBase.cpp.o.requires
CMakeFiles/BaseObj.dir/requires: CMakeFiles/BaseObj.dir/src/Source/FuncServer.cpp.o.requires

.PHONY : CMakeFiles/BaseObj.dir/requires

CMakeFiles/BaseObj.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/BaseObj.dir/cmake_clean.cmake
.PHONY : CMakeFiles/BaseObj.dir/clean

CMakeFiles/BaseObj.dir/depend:
	cd /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jk/CodeHome/Ancestral-Property-Client-jiubei /home/jk/CodeHome/Ancestral-Property-Client-jiubei /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote /home/jk/CodeHome/Ancestral-Property-Client-jiubei/cmake-build-debug-jkremote/CMakeFiles/BaseObj.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/BaseObj.dir/depend

