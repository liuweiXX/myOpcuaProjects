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
CMAKE_SOURCE_DIR = /home/liu/Desktop/opcua_modbus_server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/liu/Desktop/opcua_modbus_server/build

# Include any dependencies generated for this target.
include ../bin/CMakeFiles/modbusServer.dir/depend.make

# Include the progress variables for this target.
include ../bin/CMakeFiles/modbusServer.dir/progress.make

# Include the compile flags for this target's objects.
include ../bin/CMakeFiles/modbusServer.dir/flags.make

../bin/CMakeFiles/modbusServer.dir/main.c.o: ../bin/CMakeFiles/modbusServer.dir/flags.make
../bin/CMakeFiles/modbusServer.dir/main.c.o: ../src/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/liu/Desktop/opcua_modbus_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object ../bin/CMakeFiles/modbusServer.dir/main.c.o"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/modbusServer.dir/main.c.o   -c /home/liu/Desktop/opcua_modbus_server/src/main.c

../bin/CMakeFiles/modbusServer.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/modbusServer.dir/main.c.i"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/liu/Desktop/opcua_modbus_server/src/main.c > CMakeFiles/modbusServer.dir/main.c.i

../bin/CMakeFiles/modbusServer.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/modbusServer.dir/main.c.s"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/liu/Desktop/opcua_modbus_server/src/main.c -o CMakeFiles/modbusServer.dir/main.c.s

../bin/CMakeFiles/modbusServer.dir/modbus_operate.c.o: ../bin/CMakeFiles/modbusServer.dir/flags.make
../bin/CMakeFiles/modbusServer.dir/modbus_operate.c.o: ../src/modbus_operate.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/liu/Desktop/opcua_modbus_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object ../bin/CMakeFiles/modbusServer.dir/modbus_operate.c.o"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/modbusServer.dir/modbus_operate.c.o   -c /home/liu/Desktop/opcua_modbus_server/src/modbus_operate.c

../bin/CMakeFiles/modbusServer.dir/modbus_operate.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/modbusServer.dir/modbus_operate.c.i"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/liu/Desktop/opcua_modbus_server/src/modbus_operate.c > CMakeFiles/modbusServer.dir/modbus_operate.c.i

../bin/CMakeFiles/modbusServer.dir/modbus_operate.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/modbusServer.dir/modbus_operate.c.s"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/liu/Desktop/opcua_modbus_server/src/modbus_operate.c -o CMakeFiles/modbusServer.dir/modbus_operate.c.s

../bin/CMakeFiles/modbusServer.dir/lib/open62541.c.o: ../bin/CMakeFiles/modbusServer.dir/flags.make
../bin/CMakeFiles/modbusServer.dir/lib/open62541.c.o: ../src/lib/open62541.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/liu/Desktop/opcua_modbus_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object ../bin/CMakeFiles/modbusServer.dir/lib/open62541.c.o"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/modbusServer.dir/lib/open62541.c.o   -c /home/liu/Desktop/opcua_modbus_server/src/lib/open62541.c

../bin/CMakeFiles/modbusServer.dir/lib/open62541.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/modbusServer.dir/lib/open62541.c.i"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/liu/Desktop/opcua_modbus_server/src/lib/open62541.c > CMakeFiles/modbusServer.dir/lib/open62541.c.i

../bin/CMakeFiles/modbusServer.dir/lib/open62541.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/modbusServer.dir/lib/open62541.c.s"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/liu/Desktop/opcua_modbus_server/src/lib/open62541.c -o CMakeFiles/modbusServer.dir/lib/open62541.c.s

../bin/CMakeFiles/modbusServer.dir/lib/sqlite3.c.o: ../bin/CMakeFiles/modbusServer.dir/flags.make
../bin/CMakeFiles/modbusServer.dir/lib/sqlite3.c.o: ../src/lib/sqlite3.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/liu/Desktop/opcua_modbus_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object ../bin/CMakeFiles/modbusServer.dir/lib/sqlite3.c.o"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/modbusServer.dir/lib/sqlite3.c.o   -c /home/liu/Desktop/opcua_modbus_server/src/lib/sqlite3.c

../bin/CMakeFiles/modbusServer.dir/lib/sqlite3.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/modbusServer.dir/lib/sqlite3.c.i"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/liu/Desktop/opcua_modbus_server/src/lib/sqlite3.c > CMakeFiles/modbusServer.dir/lib/sqlite3.c.i

../bin/CMakeFiles/modbusServer.dir/lib/sqlite3.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/modbusServer.dir/lib/sqlite3.c.s"
	cd /home/liu/Desktop/opcua_modbus_server/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/liu/Desktop/opcua_modbus_server/src/lib/sqlite3.c -o CMakeFiles/modbusServer.dir/lib/sqlite3.c.s

# Object files for target modbusServer
modbusServer_OBJECTS = \
"CMakeFiles/modbusServer.dir/main.c.o" \
"CMakeFiles/modbusServer.dir/modbus_operate.c.o" \
"CMakeFiles/modbusServer.dir/lib/open62541.c.o" \
"CMakeFiles/modbusServer.dir/lib/sqlite3.c.o"

# External object files for target modbusServer
modbusServer_EXTERNAL_OBJECTS =

../bin/modbusServer: ../bin/CMakeFiles/modbusServer.dir/main.c.o
../bin/modbusServer: ../bin/CMakeFiles/modbusServer.dir/modbus_operate.c.o
../bin/modbusServer: ../bin/CMakeFiles/modbusServer.dir/lib/open62541.c.o
../bin/modbusServer: ../bin/CMakeFiles/modbusServer.dir/lib/sqlite3.c.o
../bin/modbusServer: ../bin/CMakeFiles/modbusServer.dir/build.make
../bin/modbusServer: ../bin/CMakeFiles/modbusServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/liu/Desktop/opcua_modbus_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable modbusServer"
	cd /home/liu/Desktop/opcua_modbus_server/bin && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/modbusServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
../bin/CMakeFiles/modbusServer.dir/build: ../bin/modbusServer

.PHONY : ../bin/CMakeFiles/modbusServer.dir/build

../bin/CMakeFiles/modbusServer.dir/clean:
	cd /home/liu/Desktop/opcua_modbus_server/bin && $(CMAKE_COMMAND) -P CMakeFiles/modbusServer.dir/cmake_clean.cmake
.PHONY : ../bin/CMakeFiles/modbusServer.dir/clean

../bin/CMakeFiles/modbusServer.dir/depend:
	cd /home/liu/Desktop/opcua_modbus_server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liu/Desktop/opcua_modbus_server /home/liu/Desktop/opcua_modbus_server/src /home/liu/Desktop/opcua_modbus_server/build /home/liu/Desktop/opcua_modbus_server/bin /home/liu/Desktop/opcua_modbus_server/bin/CMakeFiles/modbusServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : ../bin/CMakeFiles/modbusServer.dir/depend

