#include "export_api_sniper_sdk.h"
#include "sniper_sdk.h"


void connect_to_server_export(const char * server_host,const char* port)
{
    connect_to_server(server_host,port);
}

void send_message_export(const char * message)
{
    send_message(message);
}