#ifndef OPCUA_OPERATE_H
#define OPCUA_OPERATE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "open62541.h"
#include "cJSON.h"
#include "modbus_operate.h"

/*
#define COIL				1
#define DISCRETE_INPUT		2
#define HOLDING_REGISTER	3
#define INPUT_REGISTER		4
*/

/******************************1.开关******************************/

void UAServer_Start(char *conf);
void UAServer_Shutdown();

/******************************2.opcua-server操作******************************/

void addValueCallbackUA_Uint16_Variable(UA_Server *server, UA_NodeId currentNodeId);

int UAServer_BuildModelFromConfiguration(char *conf, UA_Server * server);

int UAServer_AddNodeFromConfiguration(cJSON *subArrayRoot, int index, UA_NodeId parent, UA_Server * server);


int findChildId(UA_Server *server, UA_NodeId parentNode, const int relativePathCnt, const UA_QualifiedName targetNameArr[], UA_NodeId *result);

void UAServer_RefreshDataDouble(const UA_NodeId *nodeid, double data, UA_Server * server);


void manuallyAddCoilVariable(UA_Server * server,char *variableName, UA_Byte value,int register_address, UA_NodeId objectNodeId,UA_NodeId *resultVariableNodeId);
void manuallyAddDiscreteInputVariable(UA_Server * server,char *variableName, UA_Byte value,int register_address, UA_NodeId objectNodeId,UA_NodeId *resultVariableNodeId);
void manuallyAddHoldingRegisterVariable(UA_Server * server,char *variableName, UA_Int16 value,int register_address, UA_NodeId objectNodeId,UA_NodeId *resultVariableNodeId);
void manuallyAddInputResigterVariable(UA_Server * server,char *variableName, UA_Int16 value,int register_address, UA_NodeId objectNodeId,UA_NodeId *resultVariableNodeId);



void manuallyAddObject(UA_Server * server, char *objectName, UA_NodeId *resultObjectNodeId);

/******************************3.opcua的数据转换********************************/


char *UAServer_UAStringToChar(UA_String str);




/****************************************************************************/
static UA_INLINE UA_ByteString loadFile(const char *const path);

#endif