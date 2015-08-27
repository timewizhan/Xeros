#ifndef _LOG_
#define _LOG_

#include "Common.h"
#include <stdarg.h>

#define MAX_BUF	128

enum E_LOG_TYPE
{
	E_LOG_CONSOLE = 0,
	E_LOG_FILE
};

enum E_LOG_LEVEL
{
	E_LOG_LEVEL_ERROR = 0,
	E_LOG_LEVEL_DEBUG,
	E_LOG_LEVEL_WARNING
};

extern char *g_pszPath;
extern E_LOG_TYPE g_eLogtype;

DWORD InitLog(E_LOG_TYPE eLogType, char *pszPath);
DWORD ErrorLog(const char *cformat, ...);
DWORD DebugLog(const char *cformat, ...);
DWORD WarningLog(const char *cformat, ...);

#endif