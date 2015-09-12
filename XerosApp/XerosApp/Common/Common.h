#ifndef _COMMON_
#define _COMMON_

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include <TlHelp32.h>
#include <tchar.h>
#include <process.h>

#include <string>
#include <vector>
#include <map>

/*
	inner unitest
*/

#include <assert.h>

/*
	Header fo ATL
*/
#include <atlbase.h>
#include <atlconv.h>

/*
	User defined
*/

#include "Type.h"
#include "Log.h"


/*
	Define Data
*/


#define DRIVER_FILE_NAME		L"xeros.sys"
#define DRIVER_OUTPUT_FILE_NAME L"keylogoutput.txt"
#define CURL_OUTPUT_FILE_NAME	L"curloutput.html"
#define LOG_FILE_NAME			L"Xeroslog.txt"


#endif