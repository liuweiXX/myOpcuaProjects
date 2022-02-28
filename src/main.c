#include <signal.h>
#include <stdlib.h>
#include "opcua_operate.h"




UA_Boolean running = true;

static void stopHandler(int sign) 
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

void addVariable(UA_Server *server) 
{
	UA_NodeId object;
    UA_NodeId value[10];
    UA_Byte a = 1;
	manuallyAddObject(server,"modbus", &object);
    manuallyAddHoldingRegisterVariable(server,"input",-1,-1,object,&value[0]);
    manuallyAddCoilVariable(server,"Coil",a,-1,object,&value[1]);	
}


int main(int argc, char* argv[]) 
{
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    if(argc < 3) {
        UA_LOG_FATAL(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Missing arguments. Arguments are "
                     "<server-certificate.der> <private-key.der> "
                     "[<trustlist1.der>, ...]");
        return EXIT_FAILURE;
    }

    /* 加载server的证书和私匙 */
    UA_ByteString certificate = loadFile(argv[1]);
    UA_ByteString privateKey  = loadFile(argv[2]);

        /* 加载信任证书 */
    size_t trustListSize = 0;
    if(argc > 3)
        trustListSize = (size_t)argc-3;
    UA_STACKARRAY(UA_ByteString, trustList, trustListSize);
    for(size_t i = 0; i < trustListSize; i++)
        trustList[i] = loadFile(argv[i+3]);


    size_t issuerListSize = 0;
    UA_ByteString *issuerList = NULL;

    UA_ByteString *revocationList = NULL;
    size_t revocationListSize = 0;



    UA_Server *server = UA_Server_new();
    UA_ServerConfig *config = UA_Server_getConfig(server);


    UA_StatusCode retval =
        UA_ServerConfig_setDefaultWithSecurityPolicies(config, 4840,
                                                       &certificate, &privateKey,
                                                       trustList, trustListSize,
                                                       issuerList, issuerListSize,
                                                       revocationList, revocationListSize);
    UA_String_deleteMembers(&config->applicationDescription.applicationUri);            
                                        
    config->applicationDescription.applicationUri = UA_STRING_ALLOC("urn:open62541.server.application");
    for (size_t i = 0; i < config->endpointsSize; ++i)
    {
        UA_String_deleteMembers(&config->endpoints[i].server.applicationUri);
        config->endpoints[i].server.applicationUri = UA_String_fromChars("urn:open62541.server.application");
    }
    
    UA_ByteString_clear(&certificate);
    UA_ByteString_clear(&privateKey);


    addVariable(server);
    
    for(size_t i = 0; i < trustListSize; i++)
        UA_ByteString_clear(&trustList[i]);
    if(retval != UA_STATUSCODE_GOOD)
        goto cleanup;

    retval = UA_Server_run(server, &running);

 cleanup:
    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
