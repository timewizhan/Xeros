#include "Log.h"

char *g_pszPath = NULL;
extern E_LOG_TYPE g_eLogtype = E_LOG_CONSOLE;

////////////////////////////////////////////////////////////////////////////////////////
DWORD InitLog(E_LOG_TYPE eLogType, char *pszPath)
{
	g_eLogtype = eLogType;

	if (eLogType == E_LOG_FILE && pszPath == NULL)
		g_eLogtype = E_LOG_CONSOLE;
	else if (eLogType == E_LOG_FILE) {
		g_pszPath = pszPath;
	}

	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD ErrorLog(const char *cformat, ...)
{
	va_list arg;
	int iCount;
	char szBuf[MAX_BUF] = { 0 };

	va_start(arg, cformat);
	iCount = vsnprintf_s(szBuf, sizeof(szBuf), cformat, arg);
	va_end(arg);

	switch (g_eLogtype)
	{
	case E_LOG_CONSOLE:
		printf("[ERROR] %s\n", szBuf);
		break;
	case E_LOG_FILE:
		/*
		later
		*/
		break;
	default:
		break;
	}

	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD DebugLog(const char *cformat, ...)
{
	va_list arg;
	int iCount;
	char szBuf[MAX_BUF] = { 0 };

	va_start(arg, cformat);
	iCount = vsnprintf_s(szBuf, sizeof(szBuf), cformat, arg);
	va_end(arg);

	switch (g_eLogtype)
	{
	case E_LOG_CONSOLE:
		printf("[DEBUG] %s\n", szBuf);
		break;
	case E_LOG_FILE:
		/*
		later
		*/
		break;
	default:
		break;
	}

	return E_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD WarningLog(const char *cformat, ...)
{
	va_list arg;
	int iCount;
	char szBuf[MAX_BUF] = { 0 };

	va_start(arg, cformat);
	iCount = vsnprintf_s(szBuf, sizeof(szBuf), cformat, arg);
	va_end(arg);

	switch (g_eLogtype)
	{
	case E_LOG_CONSOLE:
		printf("[WARNING] %s\n", szBuf);
		break;
	case E_LOG_FILE:
		/*
		later
		*/
		break;
	default:
		break;
	}

	return E_RET_SUCCESS;
}