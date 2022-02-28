#include <string.h>
#include <stdlib.h>
#include <signal.h>


#include "open62541.h"
#include "modbus_operate.h"



// ------------------opcua部分----------------------
UA_Boolean running = true;

static void stopHandler(int sign) {
	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
	running = false;
}


static void updateData(UA_Server * server, UA_NodeId modbusNodeId, int register_type, int add, int num)
{
	// 通过libmodbus将数据传输到下位机，然后响应回modbus-client，然后将数据返回到opcua的服务器的地址空间对应的节点

	if (register_type == COIL || register_type == DISCRETE_INPUT) {
		UA_Byte *tab_reg = read_coils_value(register_type, add, num);
		UA_Byte modbus_data = tab_reg[0];
		free(tab_reg);
		UA_Variant value;
		UA_Variant_setScalar(&value, &modbus_data, &UA_TYPES[UA_TYPES_BYTE]);
		UA_Server_writeValue(server, modbusNodeId, value);
	}
	else if (register_type == HOLDING_REGISTER || register_type == INPUT_REGISTER) {
		UA_UInt16 *tab_reg = read_registers_value(register_type, add, num);
		UA_UInt16 modbus_data = tab_reg[0];
		free(tab_reg);
		UA_Variant value;
		UA_Variant_setScalar(&value, &modbus_data, &UA_TYPES[UA_TYPES_UINT16]);
		UA_Server_writeValue(server, modbusNodeId, value);
	}

}

struct machineInitInfo
{
	UA_Byte coil;
	UA_Byte discrete_input;
	UA_UInt16 holding_register;
	UA_UInt16 input_register;

};


//添加一个machine对象类型的节点
UA_NodeId machineTypeId = { 1, UA_NODEIDTYPE_NUMERIC, {1001} };

static void defineObjectType(UA_Server *server)
{
	/* Define the object type for "Student" */
	UA_ObjectTypeAttributes stuAttr = UA_ObjectTypeAttributes_default;
	stuAttr.displayName = UA_LOCALIZEDTEXT("en-US", "MachineType");
	UA_Server_addObjectTypeNode(server, machineTypeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
		UA_QUALIFIEDNAME(1, "MachineType"), stuAttr,
		NULL, NULL);

	// 添加coil
	UA_VariableAttributes coilAttr = UA_VariableAttributes_default;
	UA_Boolean coil = 0;
	UA_Variant_setScalar(&coilAttr.value, &coil, &UA_TYPES[UA_TYPES_BYTE]); // 初始化为false，后续再修改
	coilAttr.displayName = UA_LOCALIZEDTEXT("en-US", "coil");
	coilAttr.accessLevel |= UA_ACCESSLEVELMASK_WRITE; // default是只读，添加写权限
	UA_NodeId coilId;
	UA_Server_addVariableNode(server, UA_NODEID_NULL, machineTypeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
		UA_QUALIFIEDNAME(1, "coil"),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), coilAttr, NULL, &coilId);
	/* Make the variable mandatory */
	UA_Server_addReference(server, coilId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
		UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

	// 添加discrete_input
	UA_VariableAttributes discrete_inputAttr = UA_VariableAttributes_default;
	UA_Boolean discrete_input = 0;
	UA_Variant_setScalar(&discrete_inputAttr.value, &discrete_input, &UA_TYPES[UA_TYPES_BYTE]); // 初始化为false，后续再修改
	discrete_inputAttr.displayName = UA_LOCALIZEDTEXT("en-US", "discrete_input");

	UA_NodeId discrete_inputId;
	UA_Server_addVariableNode(server, UA_NODEID_NULL, machineTypeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
		UA_QUALIFIEDNAME(1, "discrete_input"),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), discrete_inputAttr, NULL, &discrete_inputId);
	/* Make the variable mandatory */
	UA_Server_addReference(server, discrete_inputId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
		UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

	// 添加holding_register
	UA_VariableAttributes holding_registerAttr = UA_VariableAttributes_default;
	UA_Int16 holding_register = 0;
	UA_Variant_setScalar(&holding_registerAttr.value, &holding_register, &UA_TYPES[UA_TYPES_UINT16]); // 初始化为0，后续再修改
	holding_registerAttr.displayName = UA_LOCALIZEDTEXT("en-US", "holding_register");
	holding_registerAttr.accessLevel |= UA_ACCESSLEVELMASK_WRITE; // default是只读，添加写权限
	holding_registerAttr.valueRank = UA_VALUERANK_SCALAR;
	UA_NodeId holding_registerId;
	UA_Server_addVariableNode(server, UA_NODEID_NULL, machineTypeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
		UA_QUALIFIEDNAME(1, "holding_register"),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), holding_registerAttr, NULL, &holding_registerId);
	/* Make the variable mandatory */
	UA_Server_addReference(server, holding_registerId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
		UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

	// 添加input_register
	UA_VariableAttributes input_registerAttr = UA_VariableAttributes_default;
	UA_Int16 input_register = 0;
	UA_Variant_setScalar(&input_registerAttr.value, &input_register, &UA_TYPES[UA_TYPES_UINT16]); // 初始化为0，后续再修改
	input_registerAttr.displayName = UA_LOCALIZEDTEXT("en-US", "input_register");

	input_registerAttr.valueRank = UA_VALUERANK_SCALAR;
	UA_NodeId input_registerId;
	UA_Server_addVariableNode(server, UA_NODEID_NULL, machineTypeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
		UA_QUALIFIEDNAME(1, "input_register"),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), input_registerAttr, NULL, &input_registerId);
	/* Make the variable mandatory */
	UA_Server_addReference(server, input_registerId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
		UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
}

static int getChildIdSimplified(UA_Server *server,
	UA_NodeId parentNode,
	const int relativePathCnt,
	const UA_QualifiedName targetNameArr[],
	UA_NodeId *result)
{
	int ret = 0;

	UA_BrowsePathResult bpr = UA_Server_browseSimplifiedBrowsePath(server,
		parentNode, relativePathCnt, targetNameArr);

	if (bpr.statusCode != UA_STATUSCODE_GOOD || bpr.targetsSize < 1)
	{
		printf("error: %s\n", UA_StatusCode_name(bpr.statusCode));
		ret = -1;
	}
	else
	{
		UA_NodeId_copy(&bpr.targets[0].targetId.nodeId, result);
	}

	return ret;
}

//路径搜索，因为通过对象类型来创建实际的对象，它的nodeid是随机分配的
static int findChildId(UA_Server *server,
	UA_NodeId parentNode,
	const int relativePathCnt,
	const UA_QualifiedName targetNameArr[],
	UA_NodeId *result)
{
	int ret = 0;
	/*
	UA_BrowsePathResult UA_Server_browseSimplifiedBrowsePath(
							UA_Server *server,
							const UA_NodeId origin,					//起始节点
							size_t browsePathSize,					//相对路径间元素个数
							const UA_QualifiedName *browsePath		//目标节点的qualified名称
							)

	*/
	UA_BrowsePathResult bpr = UA_Server_browseSimplifiedBrowsePath(server,
		parentNode, relativePathCnt, targetNameArr);

	if (bpr.statusCode != UA_STATUSCODE_GOOD || bpr.targetsSize < 1)
	{
		printf("error: %s\n", UA_StatusCode_name(bpr.statusCode));
		ret = -1;
	}
	else
	{
		UA_NodeId_copy(&bpr.targets[0].targetId.nodeId, result);
	}


	UA_BrowsePathResult_deleteMembers(&bpr);

	return ret;
}


static void createMachineObjectInstance(UA_Server *server, char *name, struct machineInitInfo *info)
{
	UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
	oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);

	UA_NodeId retId;
	UA_Server_addObjectNode(server, UA_NODEID_NULL,
		UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
		UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
		UA_QUALIFIEDNAME(1, name),
		machineTypeId, /* this refers to the object type identifier */
		oAttr, NULL, &retId);

	UA_NodeId itemId;

	UA_Variant value;
	UA_QualifiedName targetNameArr[1];

	targetNameArr[0] = UA_QUALIFIEDNAME(1, "coil");
	if (getChildIdSimplified(server, retId, 1, targetNameArr, &itemId) == 0)
	{
		UA_Variant_init(&value);
		UA_Variant_setScalar(&value, &info->coil, &UA_TYPES[UA_TYPES_BYTE]);
		UA_Server_writeValue(server, itemId, value);
	}
	targetNameArr[0] = UA_QUALIFIEDNAME(1, "discrete_input");
	if (getChildIdSimplified(server, retId, 1, targetNameArr, &itemId) == 0)
	{
		UA_Variant_init(&value);
		UA_Variant_setScalar(&value, &info->discrete_input, &UA_TYPES[UA_TYPES_BYTE]);
		UA_Server_writeValue(server, itemId, value);
	}
	targetNameArr[0] = UA_QUALIFIEDNAME(1, "holding_register");
	if (getChildIdSimplified(server, retId, 1, targetNameArr, &itemId) == 0)
	{
		UA_Variant_init(&value);
		UA_Variant_setScalar(&value, &info->holding_register, &UA_TYPES[UA_TYPES_UINT16]);
		UA_Server_writeValue(server, itemId, value);
	}
	targetNameArr[0] = UA_QUALIFIEDNAME(1, "input_register");
	if (getChildIdSimplified(server, retId, 1, targetNameArr, &itemId) == 0)
	{
		UA_Variant_init(&value);
		UA_Variant_setScalar(&value, &info->input_register, &UA_TYPES[UA_TYPES_UINT16]);
		UA_Server_writeValue(server, itemId, value);
	}
	UA_QualifiedName targetNameArr1[4][2] = {
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "coil") },
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "discrete_input") },
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "holding_register") },
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "input_register") }
	};
	UA_NodeId newReturnId[4];
	int i;

	for (i = 0; i < 4; i++) {
		int ret = findChildId(server, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
			2, targetNameArr1[i], &newReturnId[i]);
		if (ret == -1)
		{
			printf("error\n");
		}
		else
		{
			updateData(server, newReturnId[i], i+1, i, 1);
		}
	}

}

static void addCurrentTimeVariable(UA_Server * server)
{
	UA_Int16 num = 0;
	UA_VariableAttributes attr = UA_VariableAttributes_default;
	attr.displayName = UA_LOCALIZEDTEXT("en-US", "modbus_data");
	attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;			// 定义为可读可写
	attr.dataType = UA_TYPES[UA_TYPES_UINT16].typeId;								// 数据类型为int16
	UA_Variant_setScalar(&attr.value, &num, &UA_TYPES[UA_TYPES_UINT16]);				// 初始value为0

	UA_NodeId currentNodeId = UA_NODEID_STRING(1, "modbus_data");		// nodeid
	UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "modbus_data");
	UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);				// parentNodeId
	UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
	UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);

	/* UA_Server_addVariableNode函数的参数：（作用：向地址空间内部添加一个变量节点）
	UA_Server *server,
	const UA_NodeId requestedNewNodeId,		请求添加的节点
	const UA_NodeId parentNodeId,			父节点
	const UA_NodeId referenceTypeId,		引用的类型
	const UA_QualifiedName browseName,		浏览的名字（非本地化）
	const UA_NodeId typeDefinition,			variableTypeNodeId
	const UA_VariableAttributes attr,		该节点的属性
	void *nodeContext,
	UA_NodeId *outNewNodeId
	*/
	UA_Server_addVariableNode(server, currentNodeId, parentNodeId,
		parentReferenceNodeId, currentName,
		variableTypeNodeId, attr, NULL, NULL);
}



// 读写回调byte
static void beforeReadByte(UA_Server *server,
	const UA_NodeId *sessionId, void *sessionContext,
	const UA_NodeId *nodeId, void *nodeContext,
	const UA_NumericRange *range, const UA_DataValue *data)
{
	if (sessionId->identifierType == UA_NODEIDTYPE_GUID &&
		sessionId->identifier.guid.data1 == 1)
	{
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "admin session --- beforeRead");
	}
	else
	{
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "client session --- beforeRead");


		UA_QualifiedName targetNameArr1[2][2] = {
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "coil") },
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "discrete_input") }
		};
		UA_NodeId newReturnId[2];
		int i;

		for (i = 0; i < 2; i++) {
			int ret = findChildId(server, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
				2, targetNameArr1[i], &newReturnId[i]);
		}
		if (nodeId->identifier.numeric == newReturnId[0].identifier.numeric) {

			updateData(server, *nodeId, COIL, 0, 1);
		}
		if (nodeId->identifier.numeric == newReturnId[1].identifier.numeric) {

			updateData(server, *nodeId, DISCRETE_INPUT, 1, 1);
		}
	}

}

static void afterWriteByte(UA_Server *server,
	const UA_NodeId *sessionId, void *sessionContext,
	const UA_NodeId *nodeId, void *nodeContext,
	const UA_NumericRange *range, const UA_DataValue *data)
{
	UA_Byte num[] = { 0 };
	if (sessionId->identifierType == UA_NODEIDTYPE_GUID &&
		sessionId->identifier.guid.data1 == 1)
	{
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "admin session --- afterWrite"); 
	}
	else
	{
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "client session --- afterWrite");


		UA_QualifiedName targetNameArr1[2] = { UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "coil") };

		UA_NodeId newReturnId;

		findChildId(server, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
			2, targetNameArr1, &newReturnId);
		if (newReturnId.identifier.numeric == nodeId->identifier.numeric) {
			UA_Variant value;
			UA_Server_readValue(server, *nodeId, &value);

			num[0] = *(UA_Byte *)value.data;
			write_registers_value(COIL, 0, 1, NULL, num);
		}
	}
}

// 调用回调函数
static void addValueCallbackUA_Byte_Variable(UA_Server *server, UA_NodeId currentNodeId)
{
	/*
	使用UA_ValueCallback创建callback，并添加2个回调函数，
		callback.onRead = beforeReadByte;
		callback.onWrite = afterWriteByte;
		读之前调用beforeReadByte函数，写之后调用afterWriteByte函数
	*/
	UA_ValueCallback callback;
	callback.onRead = beforeReadByte;
	callback.onWrite = afterWriteByte;
	UA_Server_setVariableNode_valueCallback(server, currentNodeId, callback);
}


// 读写回调int16
static void beforeRead(UA_Server *server,
	const UA_NodeId *sessionId, void *sessionContext,
	const UA_NodeId *nodeId, void *nodeContext,
	const UA_NumericRange *range, const UA_DataValue *data)
{
	if (sessionId->identifierType == UA_NODEIDTYPE_GUID &&
		sessionId->identifier.guid.data1 == 1)
	{
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "admin session --- beforeRead");
	}
	else
	{
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "client session --- beforeRead");


		UA_QualifiedName targetNameArr1[2][2] = {
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "holding_register") },
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "input_register") }
		};
		UA_NodeId newReturnId[2];
		int i;

		for (i = 0; i < 2; i++) {
			int ret = findChildId(server, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
				2, targetNameArr1[i], &newReturnId[i]);
		}
		if (nodeId->identifier.numeric == newReturnId[0].identifier.numeric) {

			updateData(server, *nodeId, HOLDING_REGISTER, 2, 1);
		}
		if (nodeId->identifier.numeric == newReturnId[1].identifier.numeric) {
			printf("------------------INPUT_REGISTER------------------------\n");
			updateData(server, *nodeId, INPUT_REGISTER, 3, 1);
		}


		// 通过nodeid来获得节点的browsename，来判断写入那个节点（未完成）



	}

}

static void afterWrite(UA_Server *server,
	const UA_NodeId *sessionId, void *sessionContext,
	const UA_NodeId *nodeId, void *nodeContext,
	const UA_NumericRange *range, const UA_DataValue *data)
{
	UA_Int16 num[] = { 0 };
	if (sessionId->identifierType == UA_NODEIDTYPE_GUID &&
		sessionId->identifier.guid.data1 == 1)
	{
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "admin session --- afterWrite");
	}
	else
	{
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "client session --- afterWrite");


		UA_QualifiedName targetNameArr1[2] = { UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "holding_register") };

		UA_NodeId newReturnId;

		findChildId(server, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
			2, targetNameArr1, &newReturnId);
		if (newReturnId.identifier.numeric == nodeId->identifier.numeric) {
			UA_Variant value;
			UA_Server_readValue(server, *nodeId, &value);

			num[0] = *(UA_Int16*)value.data;
			write_registers_value(HOLDING_REGISTER, 2, 1, num, NULL);
		}

	
	}
}

// 调用回调函数
static void addValueCallbackUA_Uint16_Variable(UA_Server *server, UA_NodeId currentNodeId)
{
	/*
	使用UA_ValueCallback创建callback，并添加2个回调函数，
		callback.onRead = beforeRead;
		callback.onWrite = afterWrite;
		读之前调用beforeRead函数，写之后调用afterWrite函数
	*/
	UA_ValueCallback callback;
	callback.onRead = beforeRead;
	callback.onWrite = afterWrite;
	UA_Server_setVariableNode_valueCallback(server, currentNodeId, callback);
}


int main(void)
{
	signal(SIGINT, stopHandler);
	signal(SIGTERM, stopHandler);

	UA_Server *server = UA_Server_new();
	UA_ServerConfig_setDefault(UA_Server_getConfig(server));

	defineObjectType(server);

	struct machineInitInfo initInfo;

	initInfo.coil = 0;
	initInfo.discrete_input = 0;
	initInfo.holding_register = 0;
	initInfo.input_register = 0;
	createMachineObjectInstance(server, "machine1", &initInfo);

	UA_QualifiedName targetNameArr1[4][2] = {
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "coil") },
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "discrete_input") },
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "holding_register") },
		{ UA_QUALIFIEDNAME(1, "machine1"), UA_QUALIFIEDNAME(1, "input_register") }
	};
	UA_NodeId newReturnId[4];
	int i;

	for (i = 0; i < 4; i++) {
		int ret = findChildId(server, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
			2, targetNameArr1[i], &newReturnId[i]);
		if (ret != -1)
		{
			printf("error");
		}
		if (i<2)
			addValueCallbackUA_Byte_Variable(server, newReturnId[i]);
		else 
			addValueCallbackUA_Uint16_Variable(server, newReturnId[i]);
	}
	UA_StatusCode retval = UA_Server_run(server, &running);

	UA_Server_delete(server);

	return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
