#include "export_api_sniper_sdk.h"
#include "sniper_sdk.h"


uint32_t connect_to_server_export(const char * server_host,const char* port)
{
   return connect_to_server(server_host,port);
}

void send_message_export(uint32_t sessionId, const char * message)
{
   send_message(sessionId,message);
}