#pragma once
#ifndef BASEINFO_H
#define BASEINFO_H


 #define SERVER_HOST "127.0.0.1"
//#define SERVER_HOST "124.222.92.5"
#define SERVER_PORT "18889"
#define GET_ALL_FILE "GET_ALL_FILE\n"

extern unsigned char SniperSHead[];
extern unsigned int sn_headLength;
extern unsigned int sn_bodyLength;
extern unsigned int sn_totalHeadLength;
extern unsigned int temp_length;

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#define WIDE_PATH_SEPARATOR L"\\"
#else
#define PATH_SEPARATOR "/"
#define WIDE_PATH_SEPARATOR L"/"
#endif

#endif