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
include CMakeFiles/parser.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/parser.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/parser.dir/flags.make

CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj: CMakeFiles/parser.dir/flags.make
CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj: CMakeFiles/parser.dir/includes_CXX.rsp
CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj: static_libs/chaiscript_parser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj"
	C:\PROGRA~2\MINGW-~1\I686-4~1.2-P\mingw32\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\parser.dir\static_libs\chaiscript_parser.cpp.obj -c D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\static_libs\chaiscript_parser.cpp

CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.i"
	C:\PROGRA~2\MINGW-~1\I686-4~1.2-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\static_libs\chaiscript_parser.cpp > CMakeFiles\parser.dir\static_libs\chaiscript_parser.cpp.i

CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.s"
	C:\PROGRA~2\MINGW-~1\I686-4~1.2-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\static_libs\chaiscript_parser.cpp -o CMakeFiles\parser.dir\static_libs\chaiscript_parser.cpp.s

CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj.requires:

.PHONY : CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj.requires

CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj.provides: CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj.requires
	$(MAKE) -f CMakeFiles\parser.dir\build.make CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj.provides.build
.PHONY : CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj.provides

CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj.provides.build: CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj


# Object files for target parser
parser_OBJECTS = \
"CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj"

# External object files for target parser
parser_EXTERNAL_OBJECTS =

libparser.a: CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj
libparser.a: CMakeFiles/parser.dir/build.make
libparser.a: CMakeFiles/parser.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libparser.a"
	$(CMAKE_COMMAND) -P CMakeFiles\parser.dir\cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\parser.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/parser.dir/build: libparser.a

.PHONY : CMakeFiles/parser.dir/build

CMakeFiles/parser.dir/requires: CMakeFiles/parser.dir/static_libs/chaiscript_parser.cpp.obj.requires

.PHONY : CMakeFiles/parser.dir/requires

CMakeFiles/parser.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\parser.dir\cmake_clean.cmake
.PHONY : CMakeFiles/parser.dir/clean

CMakeFiles/parser.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\CMakeFiles\parser.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/parser.dir/depend

