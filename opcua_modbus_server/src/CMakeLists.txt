cmake_minimum_required(VERSION 3.16)


SET(source main.c modbus_operate.c  lib/open62541.c lib/sqlite3.c)


INCLUDE_DIRECTORIES(include/modbus)
INCLUDE_DIRECTORIES(include/open62541)
INCLUDE_DIRECTORIES(include/sqlite3)

LINK_DIRECTORIES(lib)

ADD_EXECUTABLE(modbusServer ${source})

TARGET_LINK_LIBRARIES(modbusServer libmodbus.so pthread dl)

