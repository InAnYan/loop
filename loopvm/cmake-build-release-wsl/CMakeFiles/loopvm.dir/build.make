# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/Programming/Loop/loopvm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl

# Include any dependencies generated for this target.
include CMakeFiles/loopvm.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/loopvm.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/loopvm.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/loopvm.dir/flags.make

CMakeFiles/loopvm.dir/src/main.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/main.c.o: /mnt/d/Programming/Loop/loopvm/src/main.c
CMakeFiles/loopvm.dir/src/main.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/loopvm.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/main.c.o -MF CMakeFiles/loopvm.dir/src/main.c.o.d -o CMakeFiles/loopvm.dir/src/main.c.o -c /mnt/d/Programming/Loop/loopvm/src/main.c

CMakeFiles/loopvm.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/main.c > CMakeFiles/loopvm.dir/src/main.c.i

CMakeFiles/loopvm.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/main.c -o CMakeFiles/loopvm.dir/src/main.c.s

CMakeFiles/loopvm.dir/src/Loop/Chunk.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Chunk.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Chunk.c
CMakeFiles/loopvm.dir/src/Loop/Chunk.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/loopvm.dir/src/Loop/Chunk.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Chunk.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Chunk.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Chunk.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Chunk.c

CMakeFiles/loopvm.dir/src/Loop/Chunk.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Chunk.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Chunk.c > CMakeFiles/loopvm.dir/src/Loop/Chunk.c.i

CMakeFiles/loopvm.dir/src/Loop/Chunk.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Chunk.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Chunk.c -o CMakeFiles/loopvm.dir/src/Loop/Chunk.c.s

CMakeFiles/loopvm.dir/src/Loop/Object.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Object.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Object.c
CMakeFiles/loopvm.dir/src/Loop/Object.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/loopvm.dir/src/Loop/Object.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Object.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Object.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Object.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Object.c

CMakeFiles/loopvm.dir/src/Loop/Object.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Object.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Object.c > CMakeFiles/loopvm.dir/src/Loop/Object.c.i

CMakeFiles/loopvm.dir/src/Loop/Object.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Object.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Object.c -o CMakeFiles/loopvm.dir/src/Loop/Object.c.s

CMakeFiles/loopvm.dir/src/Loop/Opcode.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Opcode.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Opcode.c
CMakeFiles/loopvm.dir/src/Loop/Opcode.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/loopvm.dir/src/Loop/Opcode.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Opcode.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Opcode.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Opcode.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Opcode.c

CMakeFiles/loopvm.dir/src/Loop/Opcode.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Opcode.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Opcode.c > CMakeFiles/loopvm.dir/src/Loop/Opcode.c.i

CMakeFiles/loopvm.dir/src/Loop/Opcode.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Opcode.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Opcode.c -o CMakeFiles/loopvm.dir/src/Loop/Opcode.c.s

CMakeFiles/loopvm.dir/src/Loop/Value.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Value.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Value.c
CMakeFiles/loopvm.dir/src/Loop/Value.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/loopvm.dir/src/Loop/Value.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Value.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Value.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Value.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Value.c

CMakeFiles/loopvm.dir/src/Loop/Value.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Value.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Value.c > CMakeFiles/loopvm.dir/src/Loop/Value.c.i

CMakeFiles/loopvm.dir/src/Loop/Value.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Value.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Value.c -o CMakeFiles/loopvm.dir/src/Loop/Value.c.s

CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/VirtualMachine.c
CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.o -MF CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/VirtualMachine.c

CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/VirtualMachine.c > CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.i

CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/VirtualMachine.c -o CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.s

CMakeFiles/loopvm.dir/src/Loop/HashTable.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/HashTable.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/HashTable.c
CMakeFiles/loopvm.dir/src/Loop/HashTable.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/loopvm.dir/src/Loop/HashTable.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/HashTable.c.o -MF CMakeFiles/loopvm.dir/src/Loop/HashTable.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/HashTable.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/HashTable.c

CMakeFiles/loopvm.dir/src/Loop/HashTable.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/HashTable.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/HashTable.c > CMakeFiles/loopvm.dir/src/Loop/HashTable.c.i

CMakeFiles/loopvm.dir/src/Loop/HashTable.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/HashTable.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/HashTable.c -o CMakeFiles/loopvm.dir/src/Loop/HashTable.c.s

CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/MemoryManager.c
CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.o -MF CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/MemoryManager.c

CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/MemoryManager.c > CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.i

CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/MemoryManager.c -o CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.s

CMakeFiles/loopvm.dir/src/Loop/Error.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Error.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Error.c
CMakeFiles/loopvm.dir/src/Loop/Error.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/loopvm.dir/src/Loop/Error.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Error.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Error.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Error.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Error.c

CMakeFiles/loopvm.dir/src/Loop/Error.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Error.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Error.c > CMakeFiles/loopvm.dir/src/Loop/Error.c.i

CMakeFiles/loopvm.dir/src/Loop/Error.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Error.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Error.c -o CMakeFiles/loopvm.dir/src/Loop/Error.c.s

CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/String.c
CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/String.c

CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/String.c > CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.i

CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/String.c -o CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.s

CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/ObjectType.c
CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.o -MF CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/ObjectType.c

CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/ObjectType.c > CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.i

CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/ObjectType.c -o CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.s

CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Module.c
CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Module.c

CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Module.c > CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.i

CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Module.c -o CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.s

CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Function.c
CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Function.c

CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Function.c > CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.i

CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Function.c -o CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.s

CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Dictionary.c
CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building C object CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Dictionary.c

CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Dictionary.c > CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.i

CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Dictionary.c -o CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.s

CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Class.c
CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building C object CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Class.c

CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Class.c > CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.i

CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Class.c -o CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.s

CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Instance.c
CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building C object CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Instance.c

CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Instance.c > CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.i

CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Instance.c -o CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.s

CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/BoundMethod.c
CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building C object CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/BoundMethod.c

CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/BoundMethod.c > CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.i

CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/BoundMethod.c -o CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.s

CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Filesystem.c
CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Building C object CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Filesystem.c

CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Filesystem.c > CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.i

CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Filesystem.c -o CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.s

CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Closure.c
CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_19) "Building C object CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Closure.c

CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Closure.c > CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.i

CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Closure.c -o CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.s

CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Upvalue.c
CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_20) "Building C object CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Upvalue.c

CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Upvalue.c > CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.i

CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/Upvalue.c -o CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.s

CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.o: CMakeFiles/loopvm.dir/flags.make
CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.o: /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/List.c
CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.o: CMakeFiles/loopvm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_21) "Building C object CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.o -MF CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.o.d -o CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.o -c /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/List.c

CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/List.c > CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.i

CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/Programming/Loop/loopvm/src/Loop/Objects/List.c -o CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.s

# Object files for target loopvm
loopvm_OBJECTS = \
"CMakeFiles/loopvm.dir/src/main.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Chunk.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Object.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Opcode.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Value.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/HashTable.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Error.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.o" \
"CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.o"

# External object files for target loopvm
loopvm_EXTERNAL_OBJECTS =

loopvm: CMakeFiles/loopvm.dir/src/main.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Chunk.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Object.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Opcode.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Value.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/VirtualMachine.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/HashTable.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/MemoryManager.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Error.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Objects/String.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/ObjectType.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Objects/Module.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Objects/Function.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Objects/Dictionary.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Objects/Class.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Objects/Instance.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Objects/BoundMethod.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Filesystem.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Objects/Closure.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Objects/Upvalue.c.o
loopvm: CMakeFiles/loopvm.dir/src/Loop/Objects/List.c.o
loopvm: CMakeFiles/loopvm.dir/build.make
loopvm: libcJSON.a
loopvm: libcwalk.a
loopvm: CMakeFiles/loopvm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_22) "Linking C executable loopvm"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/loopvm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/loopvm.dir/build: loopvm
.PHONY : CMakeFiles/loopvm.dir/build

CMakeFiles/loopvm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/loopvm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/loopvm.dir/clean

CMakeFiles/loopvm.dir/depend:
	cd /mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/Programming/Loop/loopvm /mnt/d/Programming/Loop/loopvm /mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl /mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl /mnt/d/Programming/Loop/loopvm/cmake-build-release-wsl/CMakeFiles/loopvm.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/loopvm.dir/depend

