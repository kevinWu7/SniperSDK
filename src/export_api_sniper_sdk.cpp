#include "export_api_sniper_sdk.h"
#include "sniper_sdk.h"


void connect_to_server_async_export(const char * server_host,const char* port,connected_callback callback)
{
    connect_to_server_async(server_host,port,callback);
} 

void send_message_export(const char * message)
{
    send_message(message);
}