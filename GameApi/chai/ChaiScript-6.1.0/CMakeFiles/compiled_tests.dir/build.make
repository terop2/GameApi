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
include CMakeFiles/compiled_tests.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/compiled_tests.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/compiled_tests.dir/flags.make

CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj: CMakeFiles/compiled_tests.dir/flags.make
CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj: CMakeFiles/compiled_tests.dir/includes_CXX.rsp
CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj: unittests/compiled_tests.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj"
	C:\PROGRA~2\MINGW-~1\I686-4~1.2-P\mingw32\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\compiled_tests.dir\unittests\compiled_tests.cpp.obj -c D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\unittests\compiled_tests.cpp

CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.i"
	C:\PROGRA~2\MINGW-~1\I686-4~1.2-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\unittests\compiled_tests.cpp > CMakeFiles\compiled_tests.dir\unittests\compiled_tests.cpp.i

CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.s"
	C:\PROGRA~2\MINGW-~1\I686-4~1.2-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\unittests\compiled_tests.cpp -o CMakeFiles\compiled_tests.dir\unittests\compiled_tests.cpp.s

CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj.requires:

.PHONY : CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj.requires

CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj.provides: CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj.requires
	$(MAKE) -f CMakeFiles\compiled_tests.dir\build.make CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj.provides.build
.PHONY : CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj.provides

CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj.provides.build: CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj


# Object files for target compiled_tests
compiled_tests_OBJECTS = \
"CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj"

# External object files for target compiled_tests
compiled_tests_EXTERNAL_OBJECTS =

compiled_tests.exe: CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj
compiled_tests.exe: CMakeFiles/compiled_tests.dir/build.make
compiled_tests.exe: libstdlib.a
compiled_tests.exe: libparser.a
compiled_tests.exe: CMakeFiles/compiled_tests.dir/linklibs.rsp
compiled_tests.exe: CMakeFiles/compiled_tests.dir/objects1.rsp
compiled_tests.exe: CMakeFiles/compiled_tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable compiled_tests.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\compiled_tests.dir\link.txt --verbose=$(VERBOSE)
	"C:\Program Files\CMake\bin\cmake.exe" -D TEST_TARGET=compiled_tests -D TEST_EXECUTABLE=D:/cvs/gameapi-code/GameApi/GameApi/chai/ChaiScript-6.1.0/compiled_tests.exe -D TEST_EXECUTOR= -D TEST_WORKING_DIR=D:/cvs/gameapi-code/GameApi/GameApi/chai/ChaiScript-6.1.0 -D TEST_SPEC= -D TEST_EXTRA_ARGS= -D TEST_PROPERTIES= -D TEST_PREFIX=compiled. -D TEST_SUFFIX= -D TEST_LIST=compiled_tests_TESTS -D CTEST_FILE=D:/cvs/gameapi-code/GameApi/GameApi/chai/ChaiScript-6.1.0/compiled_tests_tests-b858cb2.cmake -P D:/cvs/gameapi-code/GameApi/GameApi/chai/ChaiScript-6.1.0/cmake/CatchAddTests.cmake

# Rule to build all files generated by this target.
CMakeFiles/compiled_tests.dir/build: compiled_tests.exe

.PHONY : CMakeFiles/compiled_tests.dir/build

CMakeFiles/compiled_tests.dir/requires: CMakeFiles/compiled_tests.dir/unittests/compiled_tests.cpp.obj.requires

.PHONY : CMakeFiles/compiled_tests.dir/requires

CMakeFiles/compiled_tests.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\compiled_tests.dir\cmake_clean.cmake
.PHONY : CMakeFiles/compiled_tests.dir/clean

CMakeFiles/compiled_tests.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0 D:\cvs\gameapi-code\GameApi\GameApi\chai\ChaiScript-6.1.0\CMakeFiles\compiled_tests.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/compiled_tests.dir/depend
