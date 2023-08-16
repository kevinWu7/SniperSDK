#pragma once
#ifndef BASEINFO_H
#define BASEINFO_H


#define server_host "localhost"
#ifdef _WIN32
#define  PATH_SEPARATOR "\\"
#define WIDE_PATH_SEPARATOR L"\\"
#else
#define  PATH_SEPARATOR "/"
#define WIDE_PATH_SEPARATOR L"/"
#endif
//#define server_host "124.222.92.5"

#endif