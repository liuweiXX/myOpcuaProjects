#include "opcua_operate.h"



int UAServer_NodeIDHashMapToIndexArrayPos = 0;
uint32_t UAServer_NodeIDHashMapToIndexArray[100];
/******************************1.开关******************************

void UAServer_Start(char *conf);
void UAServer_Shutdown();

******************************2.opcua-server操作******************************

static void addValueCallbackUA_Uint16_Variable(UA_Server *server, UA_NodeId currentNodeId);

int UAServer_BuildModelFromConfiguration(char *conf);

int UAServer_AddNodeFromConfiguration(cJSON *subArrayRoot, int index, UA_NodeId parent);


static int findChildId(UA_Server *server, UA_NodeId parentNode, const int relativePathCnt, const UA_QualifiedName targetNameArr[], UA_NodeId *result);

void UAServer_RefreshDataDouble(const UA_NodeId *nodeid, double data);

static void manuallyAddVariable(UA_Server * server, char *variableName, UA_Int32 value, UA_NodeId objectNodeId,UA_NodeId *resultVariableNodeId);

static void manuallyAddObject(UA_Server * server, char *objectName, UA_NodeId *resultObjectNodeId);

******************************3.opcua的数据转换********************************


char *UAServer_UAStringToChar(UA_String str);




****************************************************************************/

 /********************************************************
	【函数名称】 updateData
	【函数功能】 将modbus下位机的数据更新到opcua对应节点
	【参数】 
			server UA服务器指针
			nodeid 要读取的结点的ID
	【返回值】 无
	【开发者及日期】 刘威 2022-02-27
********************************************************/
static void updateData(UA_Server * server, UA_NodeId modbusNodeId, int register_type, int add, int num)
{
	// 通过libmodbus将数据传输到下位机，然后响应回modbus-client，然后将数据返回到opcua的服务器的地址空间对应的节点

	if (register_type == COIL || register_type == DISCRETE_INPUT) {
		UA_Byte *tab_reg = read_coils_value(register_type, add, num);
		UA_Byte modbus_data = tab_reg[0];
		free(tab_reg);// 释放内存
		UAServer_RefreshDataUA_Byte(&modbusNodeId, modbus_data, server);
	}
	else if (register_type == HOLDING_REGISTER || register_type == INPUT_REGISTER) {
		UA_UInt16 *tab_reg = read_registers_value(register_type, add, num);
		UA_UInt16 modbus_data = tab_reg[0];
		free(tab_reg);
		UAServer_RefreshDataUA_Int16(&modbusNodeId, modbus_data, server);
	}

}

//通过解析nodeid来获取与modbus对应地址
static void update2Data(UA_Server * server, UA_NodeId modbusNodeId)
{
	// 通过libmodbus将数据传输到下位机，然后响应回modbus-client，然后将数据返回到opcua的服务器的地址空间对应的节点

}

 /********************************************************
	【函数名称】 beforeRead
	【函数功能】 读取modbus下位机数据的回调函数
	【参数】 
			server UA服务器指针
			nodeid 要读取的结点的ID
	【返回值】 无
	【开发者及日期】 刘威 2022-02-27
********************************************************/
void beforeRead(UA_Server *server,
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
		updateData(server, *nodeId,3,1,1);

/*
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

			//updateData(server, *nodeId, HOLDING_REGISTER, 2, 1);
		}
		if (nodeId->identifier.numeric == newReturnId[1].identifier.numeric) {
			printf("------------------INPUT_REGISTER------------------------\n");
			//updateData(server, *nodeId, INPUT_REGISTER, 3, 1);
		}
		// 通过nodeid来获得节点的browsename，来判断写入那个节点（未完成）
*/
	}

}
 /********************************************************
	【函数名称】 afterWrite
	【函数功能】 获取opcua客户端写入数据的回调函数
	【参数】 
			server UA服务器指针
			nodeid 要读取的结点的ID
	【返回值】 无
	【开发者及日期】 刘威 2022-02-27
********************************************************/

void afterWrite(UA_Server *server,
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



		UA_Variant value;
		UA_Server_readValue(server, *nodeId, &value);

		num[0] = *(UA_Int16*)value.data;
		write_registers_value(HOLDING_REGISTER, 1, 1, num, NULL);


	
	}
}
 /********************************************************
	【函数名称】 addValueCallbackUA_Uint16_Variable
	【函数功能】 为UA服务器添加变量节点的回调函数
	【参数】 
			server 			UA服务器指针
			currentNodeId 	要读取的结点的ID
	【返回值】 无
	【开发者及日期】 刘威 2022-02-27
********************************************************/
void addValueCallbackUA_Variable(UA_Server *server, UA_NodeId currentNodeId)
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



/********************************************************
	【函数名称】 UAServer_Start
	【函数功能】 启动UA服务器
	【参数】 
		conf 配置文件指针
	【返回值】 无
	【开发者及日期】 刘威 2022-02-27
	【更改记录】未完成
********************************************************/
void UAServer_Start(char *conf)
{
/*
	server = UA_Server_new();
	printf("UA Server is Running! 1");
	fflush(stdout);										// 目的是清空缓冲，强制结果马上显示到屏幕上。
	//UA_ServerConfig_setDefault(UA_Server_getConfig(server));
	UA_ServerConfig_setMinimal(UA_Server_getConfig(server), 4840, NULL);
	printf("UA Server is Running! 2");
	fflush(stdout);
	running = true;
	//UAServer_AddModelToServer(server);
	//free(conf);
	int err = UAServer_BuildModelFromConfiguration(conf);
	printf("UA Server is Running! 3");
	fflush(stdout);
	//printf("New NodeID's hash are %d and %d, totle:%d.\n", UA_NodeId_hash(UAServer_NodeIDMapToIndexArray[0]),UA_NodeId_hash(UAServer_NodeIDMapToIndexArray[1]),UAServer_NodeIDMapToIndexArrayPos);
	//printf(UAServer_NodeIDMapToIndexArray[0]->identifier);
	// curServerConfig = UA_Server_getConfig(server);
	UA_StatusCode status = UA_Server_run(server, &running);
	printf("UA Server is Running! 4");

	UA_Server_delete(server);
	//UA_ServerConfig_delete(config);
*/
}
/********************************************************
	【函数名称】 UAServer_Shutdown
	【函数功能】 关闭UA服务器
	【参数】 无
	【返回值】 无
	【开发者及日期】 刘威 2022-02-27
    【更改记录】未完成
********************************************************/
void UAServer_Shutdown()
{
//	running = false;
}


/********************************************************
	【函数名称】 UAServer_BuildModelFromConfiguration
	【函数功能】 从配置文件创建地址空间模型
	【参数】 
		conf 配置文件指针
	【返回值】
		0 创建成功
		other 失败
	【开发者及日期】 刘威 2022-02-27
	【更改记录】
********************************************************/
int UAServer_BuildModelFromConfiguration(char *conf, UA_Server * server)
{
	int nodeCount = 0, i = 0;
	cJSON *root = cJSON_Parse(conf); //将Json串解析成为cJSON格式
	free(conf);						 //释放文件内容指针
	if (cJSON_HasObjectItem(root, "AddressSpace") == 0)
		return -1;													 //判断配置文件是否合法：存在“AddressSpace”字段
	cJSON *addressSpace = cJSON_GetObjectItem(root, "AddressSpace"); //获取上述字段对象（数组）
	nodeCount = cJSON_GetArraySize(addressSpace);					 //下标从0开始				//统计数组元素个数
	if (nodeCount <= 0)
		return -2; //字段错误或无元素
	for (i = 0; i < nodeCount; i++)
	{ //循环：为每个元素生成节点
		int err = UAServer_AddNodeFromConfiguration(addressSpace, i, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), server);
		if (err != 0)
			return err;
	}
	#ifdef MECHANICAL_ARM
	addMethod(server, UAServer_ArmCtrlMethodCallback);
	#endif
	cJSON_Delete(root); //删除JSON指针，释放空间
	return 0;
}

/********************************************************
	【函数名称】 UAServer_AddNodeFromConfiguration
	【函数功能】 从配置文件创建地址空间结点
	【参数】 
		subArrayRoot 子节点数组json字符串
		index 要创建的节点在数组中的下标
		parent 父节点ID
	【返回值】 】
		0 创建成功
		other 失败
	【开发者及日期】 刘威 2022-02-27
	【更改记录】
********************************************************/
int UAServer_AddNodeFromConfiguration(cJSON *subArrayRoot, int index, UA_NodeId parent, UA_Server * server)
{
	cJSON *node = cJSON_GetArrayItem(subArrayRoot, index); //获取元素指针:数组的index项
	//printf("%s",cJSON_Print(cJSON_GetObjectItem(node,"NodeID_IDType")));
	UA_NodeId thisNodeID; //构造node的成员：nodeID
	if (strcmp(cJSON_GetObjectItem(node, "NodeID_IDType")->valuestring, "String") == 0)
	{																																					  //判断节点ID类型：字符串
		thisNodeID = UA_NODEID_STRING(cJSON_GetObjectItem(node, "NodeID_NamespaceIndex")->valueint, cJSON_GetObjectItem(node, "NodeID_ID")->valuestring); //按照字符串生成节点ID
	}
	else if (strcmp(cJSON_GetObjectItem(node, "NodeID_IDType")->valuestring, "Numeric") == 0)															//判断节点ID类型：数字
		thisNodeID = UA_NODEID_NUMERIC(cJSON_GetObjectItem(node, "NodeID_NamespaceIndex")->valueint, cJSON_GetObjectItem(node, "NodeID_ID")->valueint); //按照Numeric生成节点ID
	UA_NodeId refNodeID = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);																						//获取引用类型
	if (strcmp(cJSON_GetObjectItem(node, "ReferenceTypeId")->valuestring, "Organizes") == 0)															//引用类型：组织（在文件夹）
		refNodeID = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
	else if (strcmp(cJSON_GetObjectItem(node, "ReferenceTypeId")->valuestring, "HsaComponent") == 0) //引用类型：组件
		refNodeID = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
	else if (strcmp(cJSON_GetObjectItem(node, "ReferenceTypeId")->valuestring, "HasProperty") == 0) //引用类型：属性
		refNodeID = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
	if (strcmp(cJSON_GetObjectItem(node, "NodeClass")->valuestring, "Object") == 0)
	{																											//判断节点类型:obj
		UA_ObjectAttributes attrObj = UA_ObjectAttributes_default;												//构造Node的成员：节点属性
		attrObj.description = UA_LOCALIZEDTEXT("en-US", cJSON_GetObjectItem(node, "Description")->valuestring); //属性：描述
		attrObj.displayName = UA_LOCALIZEDTEXT("en-US", cJSON_GetObjectItem(node, "DisplayName")->valuestring);
		UA_Server_addObjectNode(server, //添加到地址空间
								thisNodeID,
								parent,
								refNodeID,
								UA_QUALIFIEDNAME(1, cJSON_GetObjectItem(node, "QualifiedName")->valuestring),
								UA_NODEID_NULL,
								attrObj, NULL, NULL);
	}
	else if (strcmp(cJSON_GetObjectItem(node, "NodeClass")->valuestring, "Variable") == 0)
	{																											//判断节点类型：容器
		UA_VariableAttributes attrVar = UA_VariableAttributes_default;											//构造Node的成员：容器节点属性
		attrVar.description = UA_LOCALIZEDTEXT("en-US", cJSON_GetObjectItem(node, "Description")->valuestring); //属性：描述
		attrVar.displayName = UA_LOCALIZEDTEXT("en-US", cJSON_GetObjectItem(node, "DisplayName")->valuestring);
		if (strcmp(cJSON_GetObjectItem(node, "DataType")->valuestring, "Double") == 0)
		{ //判断容器数据类型：double
			UA_Double *temp = (UA_Double *)malloc(sizeof(UA_Double));
			attrVar.dataType = UA_NODEID_NUMERIC(0, UA_NS0ID_DOUBLE);
			*temp = cJSON_GetObjectItem(node, "InitialValue")->valuedouble;
			//printf("------------------------%x\n",cJSON_GetObjectItem(node,"InitialValue")->type);
			UA_Variant_setScalar(&attrVar.value, temp, &UA_TYPES[UA_TYPES_DOUBLE]);
		}
		else
			return -4; ///@@@@@@@@@@@@@@@@@@不同的数据类型
		UA_Server_addVariableNode(server,
								  thisNodeID,
								  parent,
								  UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
								  UA_QUALIFIEDNAME(1, cJSON_GetObjectItem(node, "QualifiedName")->valuestring),
								  UA_NODEID_NULL,
								  attrVar, NULL, NULL);
		printf("Has add NodeID %s, it's hash is %d, index is %d.\n", cJSON_GetObjectItem(node, "DisplayName")->valuestring, UA_NodeId_hash(&thisNodeID), UAServer_NodeIDHashMapToIndexArrayPos);
		UAServer_NodeIDHashMapToIndexArray[UAServer_NodeIDHashMapToIndexArrayPos++] = UA_NodeId_hash(&thisNodeID);
		if (UAServer_NodeIDHashMapToIndexArrayPos == 100)
			UAServer_NodeIDHashMapToIndexArrayPos = 99;
		//UAServer_AddVariableCallback(thisNodeID);
	}
	else
		return -3; ///####################不同的节点类型
	if (cJSON_HasObjectItem(node, "ChildNode"))
	{ //检测是否有“ChildNode”字段
		int nodeCount = 0, i = 0;
		nodeCount = cJSON_GetArraySize(cJSON_GetObjectItem(node, "ChildNode"));
		for (i = 0; i < nodeCount; i++)
		{ //循环：为每个元素生成节点
			int err = UAServer_AddNodeFromConfiguration(cJSON_GetObjectItem(node, "ChildNode"), i, thisNodeID, server);
			if (err != 0)
				return err;
		}
	}
	return 0;
}

/********************************************************
	【函数名称】 findChildId
	【函数功能】 路径搜索，已知路径上的qualified名称，从开始节点搜索该节点的nodeid
	【参数】 
		parentNode 路径搜索开始的节点
		relativePathCnt 除开始的节点，相对路径中元素的个数
		targetNameArr[] 相对路径下的node元素的qualified名称数组
		result 存放目标节点的nodeid指针
	【返回值】 
		0: 正常
	   <0: 异常
	【开发者及日期】 刘威 2022-02-27
	【说明】 
********************************************************/
int findChildId(UA_Server *server,
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




/********************************************************
	【函数名称】 UAServer_RefreshDataDouble
	【函数功能】 将double数据刷新到结点中
	【参数】 
		nodeid 要刷新数据的节点ID
		data 要刷新的数据
	【返回值】 无
	【开发者及日期】 刘威 2022-02-27
********************************************************/
void UAServer_RefreshDataDouble(const UA_NodeId *nodeid, double data, UA_Server * server)
{
	UA_Variant var;
	UA_Variant_setScalar(&var, &data, &UA_TYPES[UA_TYPES_DOUBLE]);
	UA_Server_writeValue(server, *nodeid, var);
}

void UAServer_RefreshDataUA_Byte(const UA_NodeId *nodeid, UA_Byte data, UA_Server * server)
{
	UA_Variant var;
	UA_Variant_setScalar(&var, &data, &UA_TYPES[UA_TYPES_BYTE]);
	UA_Server_writeValue(server, *nodeid, var);
}

void UAServer_RefreshDataUA_Int32(const UA_NodeId *nodeid, UA_Int32 data, UA_Server * server)
{
	UA_Variant var;
	UA_Variant_setScalar(&var, &data, &UA_TYPES[UA_TYPES_INT32]);
	UA_Server_writeValue(server, *nodeid, var);
}

void UAServer_RefreshDataUA_Int16(const UA_NodeId *nodeid, UA_Int16 data, UA_Server * server)
{
	UA_Variant var;
	UA_Variant_setScalar(&var, &data, &UA_TYPES[UA_TYPES_INT16]);
	UA_Server_writeValue(server, *nodeid, var);
}

/********************************************************
	【函数名称】 getLocalTime
	【函数功能】 获得当地时间
	【参数】 

	【返回值】 UA_DateTime 类型
	【开发者及日期】 刘威 2022-02-27
********************************************************/
UA_DateTime getLocalTime()
{
	UA_DateTime tOffset = UA_DateTime_localTimeUtcOffset() + UA_DateTime_now();
	return tOffset;
}
/********************************************************
	【函数名称】 manuallyAddObject
	【函数功能】 向地址空间内添加一个对象
	【参数】 
		objectName
		resultObjectNodeId
	【返回值】 UA_DateTime 类型
	【开发者及日期】 刘威 2022-02-27
********************************************************/
void manuallyAddObject(UA_Server * server, char *objectName, UA_NodeId *resultObjectNodeId)
{
	UA_NodeId theNodeId = UA_NODEID_STRING(1, objectName);
    UA_ObjectAttributes stuAttr = UA_ObjectAttributes_default;
    stuAttr.displayName = UA_LOCALIZEDTEXT("en-US", objectName);
    UA_Server_addObjectNode(server, theNodeId,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            UA_QUALIFIEDNAME(1, objectName), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                            stuAttr, NULL, resultObjectNodeId);
}

/**************************************
#define COIL				1
#define DISCRETE_INPUT		2
#define HOLDING_REGISTER	3
#define INPUT_REGISTER		4
**************************************/


/********************************************************
	【函数名称】 manuallyAdd_______Variable
	【函数功能】 向对象添加一个变量
	【参数】 
		variableName
		value
		objectNodeId
		resultVariableNodeId
	【返回值】 
	【开发者及日期】 刘威 2022-02-27
********************************************************/

void manuallyAddCoilVariable(
	UA_Server * server,
	char *variableName, 
	UA_Byte value,
	int register_address, 
	UA_NodeId objectNodeId,
	UA_NodeId *resultVariableNodeId
	)
{
	UA_NodeId theNodeId = UA_NODEID_STRING(1, variableName);
    UA_VariableAttributes nameAttr = UA_VariableAttributes_default;

    UA_Byte theValue = value;
    UA_Variant_setScalar(&nameAttr.value, &theValue, &UA_TYPES[UA_TYPES_BYTE]);

    nameAttr.displayName = UA_LOCALIZEDTEXT("en-US", variableName);
	nameAttr.description = UA_LOCALIZEDTEXT("en-US",variableName);
    nameAttr.dataType = UA_TYPES[UA_TYPES_BYTE].typeId;
    nameAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;


    UA_Server_addVariableNode(server, theNodeId, objectNodeId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, variableName),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), nameAttr, NULL, &resultVariableNodeId);
	
}

void manuallyAddDiscreteInputVariable(
	UA_Server * server,
	char *variableName, 
	UA_Byte value,
	int register_address, 
	UA_NodeId objectNodeId,
	UA_NodeId *resultVariableNodeId
	)
{
	UA_NodeId theNodeId = UA_NODEID_STRING(1, variableName);
    UA_VariableAttributes nameAttr = UA_VariableAttributes_default;
    UA_Byte theValue = value;
    UA_Variant_setScalar(&nameAttr.value, &theValue, &UA_TYPES[UA_TYPES_BYTE]);

    nameAttr.displayName = UA_LOCALIZEDTEXT("en-US", variableName);
	nameAttr.description = UA_LOCALIZEDTEXT("en-US",variableName);
    nameAttr.dataType = UA_TYPES[UA_TYPES_BYTE].typeId;



    UA_Server_addVariableNode(server, theNodeId, objectNodeId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, variableName),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), nameAttr, NULL, &resultVariableNodeId);
}

void manuallyAddHoldingRegisterVariable(
	UA_Server * server,
	char *variableName, 
	UA_Int16 value, 
	int register_address,
	UA_NodeId objectNodeId,
	UA_NodeId *resultVariableNodeId
	)
{
	UA_NodeId theNodeId = UA_NODEID_STRING(1, variableName);
    UA_VariableAttributes nameAttr = UA_VariableAttributes_default;
    UA_Int16 theValue = value;
    UA_Variant_setScalar(&nameAttr.value, &theValue, &UA_TYPES[UA_TYPES_INT16]);

    nameAttr.displayName = UA_LOCALIZEDTEXT("en-US", variableName);
	nameAttr.description = UA_LOCALIZEDTEXT("en-US",variableName);
    nameAttr.dataType = UA_TYPES[UA_TYPES_INT16].typeId;
    nameAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;


    UA_Server_addVariableNode(server, theNodeId, objectNodeId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, variableName),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), nameAttr, NULL, &resultVariableNodeId);
	
		if(register_address < 0)
	{
		printf("the node not connect INPUT_REGISTER \n");
	}
	else
	{
		addValueCallbackUA_Variable(server, theNodeId);
	}


}

void manuallyAddInputRegisterVariable(
	UA_Server * server,
	char *variableName, 
	UA_Int16 value, 
	int register_address,
	UA_NodeId objectNodeId,
	UA_NodeId *resultVariableNodeId
	)
{
	UA_NodeId theNodeId = UA_NODEID_STRING(1, variableName);
    UA_VariableAttributes nameAttr = UA_VariableAttributes_default;
    UA_Int16 theValue = value;
    UA_Variant_setScalar(&nameAttr.value, &theValue, &UA_TYPES[UA_TYPES_INT16]);

    nameAttr.displayName = UA_LOCALIZEDTEXT("en-US", variableName);
	nameAttr.description = UA_LOCALIZEDTEXT("en-US",variableName);
    nameAttr.dataType = UA_TYPES[UA_TYPES_INT16].typeId;


    UA_Server_addVariableNode(server, theNodeId, objectNodeId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, variableName),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), nameAttr, NULL, &resultVariableNodeId);
	if(register_address < 0)
	{
		printf("the node not connect INPUT_REGISTER \n");
	}
	else
	{

	}

}


/******************************opcua数据转换成普通数据******************************/



 /********************************************************
	【函数名称】 UAServer_UAStringToChar
	【函数功能】 将UA_String字符串转换为char*
	【参数】 str 要转换的字符串
	【返回值】 转换后的字符串指针 
	【注意】 需要调用后由调用者释放
	【开发者及日期】 刘威 2022-02-27
********************************************************/
char *UAServer_UAStringToChar(UA_String str)
{
	char *p = (char *)malloc(sizeof(char) * (str.length + 1));
	int i = 0;
	for (; i < str.length; i++)
	{
		p[i] = str.data[i];
	}
	p[i] = (char)0;
	return p;
}




 /********************************************************
	【函数名称】 loadFile
	【函数功能】 读取证书和私匙
	【参数】
			path 要转换的字符串

	【返回值】 fileContents 

	【开发者及日期】 刘威 2022-02-27
********************************************************/
static UA_INLINE UA_ByteString loadFile(const char *const path) 
{
    UA_ByteString fileContents = UA_STRING_NULL;


    FILE *fp = fopen(path, "rb");
    if(!fp) {
        errno = 0; 
        return fileContents;
    }


    fseek(fp, 0, SEEK_END);						// 把fp指针退回到离文件结尾0字节处。
    fileContents.length = (size_t)ftell(fp);	// 返回指定流的文件指示器指针的当前值。 如果是二进制流，它将从头开始以字节为单位返回位置。
    fileContents.data = (UA_Byte *)UA_malloc(fileContents.length * sizeof(UA_Byte));
    if(fileContents.data) {
        fseek(fp, 0, SEEK_SET);					// 把fp指针移动到离文件开头0字节处
        size_t read = fread(fileContents.data, sizeof(UA_Byte), fileContents.length, fp);
        if(read != fileContents.length)			// 从文件流中读数据，最多读取count个项，每个项size个字节，如果调用成功返回实际读取到的项个数
            UA_ByteString_clear(&fileContents);
    } else {
        fileContents.length = 0;
    }
    fclose(fp);

    return fileContents;
}














/********************************************************************************/

/*
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

static void addValueCallbackUA_Byte_Variable(UA_Server *server, UA_NodeId currentNodeId)
{
	UA_ValueCallback callback;
	callback.onRead = beforeReadByte;
	callback.onWrite = afterWriteByte;
	UA_Server_setVariableNode_valueCallback(server, currentNodeId, callback);
}
*/