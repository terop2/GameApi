# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.8

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0

# Include any dependencies generated for this target.
include CMakeFiles/stdlib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/stdlib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/stdlib.dir/flags.make

CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj: CMakeFiles/stdlib.dir/flags.make
CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj: CMakeFiles/stdlib.dir/includes_CXX.rsp
CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj: static_libs/chaiscript_stdlib.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj"
	C:\PROGRA~2\MINGW-~1\I686-4~1.2-P\mingw32\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\stdlib.dir\static_libs\chaiscript_stdlib.cpp.obj -c D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\static_libs\chaiscript_stdlib.cpp

CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.i"
	C:\PROGRA~2\MINGW-~1\I686-4~1.2-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\static_libs\chaiscript_stdlib.cpp > CMakeFiles\stdlib.dir\static_libs\chaiscript_stdlib.cpp.i

CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.s"
	C:\PROGRA~2\MINGW-~1\I686-4~1.2-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\static_libs\chaiscript_stdlib.cpp -o CMakeFiles\stdlib.dir\static_libs\chaiscript_stdlib.cpp.s

CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj.requires:

.PHONY : CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj.requires

CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj.provides: CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj.requires
	$(MAKE) -f CMakeFiles\stdlib.dir\build.make CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj.provides.build
.PHONY : CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj.provides

CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj.provides.build: CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj


# Object files for target stdlib
stdlib_OBJECTS = \
"CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj"

# External object files for target stdlib
stdlib_EXTERNAL_OBJECTS =

libstdlib.a: CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj
libstdlib.a: CMakeFiles/stdlib.dir/build.make
libstdlib.a: CMakeFiles/stdlib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libstdlib.a"
	$(CMAKE_COMMAND) -P CMakeFiles\stdlib.dir\cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\stdlib.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/stdlib.dir/build: libstdlib.a

.PHONY : CMakeFiles/stdlib.dir/build

CMakeFiles/stdlib.dir/requires: CMakeFiles/stdlib.dir/static_libs/chaiscript_stdlib.cpp.obj.requires

.PHONY : CMakeFiles/stdlib.dir/requires

CMakeFiles/stdlib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\stdlib.dir\cmake_clean.cmake
.PHONY : CMakeFiles/stdlib.dir/clean

CMakeFiles/stdlib.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\CMakeFiles\stdlib.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/stdlib.dir/depend

