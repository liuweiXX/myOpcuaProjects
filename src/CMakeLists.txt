cmake_minimum_required(VERSION 3.16)


SET(source main.c lib/opcua_operate.c lib/modbus_operate.c lib/sqlite3_operate.c lib/open62541.c lib/sqlite3.c lib/cJSON.c)


INCLUDE_DIRECTORIES(include/modbus)
INCLUDE_DIRECTORIES(include/open62541)
INCLUDE_DIRECTORIES(include/sqlite3)
INCLUDE_DIRECTORIES(include/cjson)

LINK_DIRECTORIES(lib)

ADD_EXECUTABLE(modbusServer ${source})

TARGET_LINK_LIBRARIES(modbusServer libmodbus.so pthread dl)

