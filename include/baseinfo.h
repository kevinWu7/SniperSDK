#pragma once
#ifndef BASEINFO_H
#define BASEINFO_H

/*#ifdef __linux__
#define server_host "124.222.92.5"
#elif defined(_WIN32)
#define server_host "localhost"
#elif defined(__APPLE__)
#define server_host "localhost"
#endif*/

// #define server_host "127.0.0.1"
#define server_host "124.222.92.5"

extern unsigned char SniperHead[];
extern unsigned int sn_headLength;
extern unsigned int sn_bodyLength;
extern unsigned int sn_totalHeadLength;

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#define WIDE_PATH_SEPARATOR L"\\"
#else
#define PATH_SEPARATOR "/"
#define WIDE_PATH_SEPARATOR L"/"
#endif

#endif