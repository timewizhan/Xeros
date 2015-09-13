#include "Log.h"

char *g_pszPath = NULL;
FILE	*pLogFile;
extern E_LOG_TYPE g_eLogtype = E_LOG_CONSOLE;

std::string strLogFileFullPath;

////////////////////////////////////////////////////////////////////////////////////////
DWORD InitLog(E_LOG_TYPE eLogType)
{
	g_eLogtype = eLogType;
	pLogFile = NULL;

	if (eLogType == E_LOG_CONSOLE)
		g_eLogtype = E_LOG_CONSOLE;
	else if (eLogType == E_LOG_FILE) {
		TCHAR lpwBuffer[512] = { 0 };
		GetCurrentDirectory(512, lpwBuffer);
		std::wstring strBuffer = lpwBuffer;
		std::wstring strFileName = LOG_FILE_NAME;
		strBuffer += L"\\" + strFileName;

		strLogFileFullPath.assign(strBuffer.begin(), strBuffer.end());
		int iRet;
		iRet = ::fopen_s(&pLogFile, strLogFileFullPath.c_str(), "a+");
		if (iRet != 0) {
			return E_RET_FAIL;
		}
		::fclose(pLogFile);
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
		int iRet;
		iRet = ::fopen_s(&pLogFile, strLogFileFullPath.c_str(), "a+");
		if (iRet != 0) {
			return E_RET_FAIL;
		}
		fputs(szBuf, pLogFile);
		fputs("\n", pLogFile);
		::fclose(pLogFile);
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
		int iRet;
		iRet = ::fopen_s(&pLogFile, strLogFileFullPath.c_str(), "a+");
		if (iRet != 0) {
			return E_RET_FAIL;
		}
		fputs(szBuf, pLogFile);
		fputs("\n", pLogFile);
		::fclose(pLogFile);
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
		int iRet;
		iRet = ::fopen_s(&pLogFile, strLogFileFullPath.c_str(), "a+");
		if (iRet != 0) {
			return E_RET_FAIL;
		}
		fputs(szBuf, pLogFile);
		::fclose(pLogFile);
		break;
	default:
		break;
	}

	return E_RET_SUCCESS;
}