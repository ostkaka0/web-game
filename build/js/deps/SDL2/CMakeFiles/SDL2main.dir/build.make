# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.8

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
CMAKE_SOURCE_DIR = /home/ost/git/web-game

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ost/git/web-game/build/js

# Include any dependencies generated for this target.
include deps/SDL2/CMakeFiles/SDL2main.dir/depend.make

# Include the progress variables for this target.
include deps/SDL2/CMakeFiles/SDL2main.dir/progress.make

# Include the compile flags for this target's objects.
include deps/SDL2/CMakeFiles/SDL2main.dir/flags.make

deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o: deps/SDL2/CMakeFiles/SDL2main.dir/flags.make
deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o: deps/SDL2/CMakeFiles/SDL2main.dir/includes_C.rsp
deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o: ../../deps/SDL2/src/main/dummy/SDL_dummy_main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ost/git/web-game/build/js/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o"
	cd /home/ost/git/web-game/build/js/deps/SDL2 && /home/ost/programs/emsdk/emscripten/1.37.14/emcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o   -c /home/ost/git/web-game/deps/SDL2/src/main/dummy/SDL_dummy_main.c

deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.i"
	cd /home/ost/git/web-game/build/js/deps/SDL2 && /home/ost/programs/emsdk/emscripten/1.37.14/emcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ost/git/web-game/deps/SDL2/src/main/dummy/SDL_dummy_main.c > CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.i

deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.s"
	cd /home/ost/git/web-game/build/js/deps/SDL2 && /home/ost/programs/emsdk/emscripten/1.37.14/emcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ost/git/web-game/deps/SDL2/src/main/dummy/SDL_dummy_main.c -o CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.s

deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o.requires:

.PHONY : deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o.requires

deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o.provides: deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o.requires
	$(MAKE) -f deps/SDL2/CMakeFiles/SDL2main.dir/build.make deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o.provides.build
.PHONY : deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o.provides

deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o.provides.build: deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o


# Object files for target SDL2main
SDL2main_OBJECTS = \
"CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o"

# External object files for target SDL2main
SDL2main_EXTERNAL_OBJECTS =

deps/SDL2/libSDL2main.a: deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o
deps/SDL2/libSDL2main.a: deps/SDL2/CMakeFiles/SDL2main.dir/build.make
deps/SDL2/libSDL2main.a: deps/SDL2/CMakeFiles/SDL2main.dir/objects1.rsp
deps/SDL2/libSDL2main.a: deps/SDL2/CMakeFiles/SDL2main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ost/git/web-game/build/js/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libSDL2main.a"
	cd /home/ost/git/web-game/build/js/deps/SDL2 && $(CMAKE_COMMAND) -P CMakeFiles/SDL2main.dir/cmake_clean_target.cmake
	cd /home/ost/git/web-game/build/js/deps/SDL2 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SDL2main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
deps/SDL2/CMakeFiles/SDL2main.dir/build: deps/SDL2/libSDL2main.a

.PHONY : deps/SDL2/CMakeFiles/SDL2main.dir/build

deps/SDL2/CMakeFiles/SDL2main.dir/requires: deps/SDL2/CMakeFiles/SDL2main.dir/src/main/dummy/SDL_dummy_main.c.o.requires

.PHONY : deps/SDL2/CMakeFiles/SDL2main.dir/requires

deps/SDL2/CMakeFiles/SDL2main.dir/clean:
	cd /home/ost/git/web-game/build/js/deps/SDL2 && $(CMAKE_COMMAND) -P CMakeFiles/SDL2main.dir/cmake_clean.cmake
.PHONY : deps/SDL2/CMakeFiles/SDL2main.dir/clean

deps/SDL2/CMakeFiles/SDL2main.dir/depend:
	cd /home/ost/git/web-game/build/js && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ost/git/web-game /home/ost/git/web-game/deps/SDL2 /home/ost/git/web-game/build/js /home/ost/git/web-game/build/js/deps/SDL2 /home/ost/git/web-game/build/js/deps/SDL2/CMakeFiles/SDL2main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : deps/SDL2/CMakeFiles/SDL2main.dir/depend
