#include "RealTimeCheck.h"

///////////////////////////////////////////////////////////////////////////////////
CRealTimeCheck::CRealTimeCheck()
{
}

///////////////////////////////////////////////////////////////////////////////////
CRealTimeCheck::~CRealTimeCheck()
{
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CRealTimeCheck::StartRealTimeCheck()
{
	DWORD dwRet = E_RET_SUCCESS;
	
	try
	{
		ST_RTC_OPEN_PROCESS stRTCOpenProcess;
		dwRet = GetCurrentOpenProcess(stRTCOpenProcess);
		if (dwRet != E_RET_SUCCESS) {
			throw std::exception("Fail to get current open process");
		}
		dwRet = CheckForeGround(stRTCOpenProcess);
		if (dwRet == E_RET_CHECK_FAIL) {
			/*
				nothing to do.
				cannot find browser on foreground
			*/
			DebugLog("Not find browser on foreground");
		}
		else if (dwRet != E_RET_SUCCESS) {
			throw std::exception("Fail to check foreground");
		}
	}
	catch (std::exception &e)
	{
		ErrorLog("%s", e.what());
		dwRet = E_RET_FAIL;

	}
	return dwRet;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CRealTimeCheck::GetCurrentOpenProcess(ST_RTC_OPEN_PROCESS &refstRTCOpenProcess)
{
	HANDLE hSnapshot;
	hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		ErrorLog("Fail to create tool help");
		return E_RET_FAIL;
	}
	
	PROCESSENTRY32 stProcess32;
	stProcess32.dwSize = sizeof(PROCESSENTRY32);
	ST_RTC_OPEN_PROCESS stRTCOpenProcess;
	BOOL bRet = TRUE, bFirst = TRUE;
	while (bRet) {
		if (bFirst) {
			bRet = ::Process32First(hSnapshot, &stProcess32);
			bFirst = FALSE;
		}
		else {
			bRet = ::Process32Next(hSnapshot, &stProcess32);
		}

		if (!bRet) {
			ErrorLog("Fail to get process\n");
			::CloseHandle(hSnapshot);
			break;
		}
		std::pair<std::string, DWORD> mapArgu;
#ifdef _UNICODE
		std::wstring strwExeFile = stProcess32.szExeFile;
		std::string strExeFile(strwExeFile.begin(), strwExeFile.end());
#else
		std::string strExeFile = stProcess32.szExeFile;
#endif
		DWORD dwProcessID = stProcess32.th32ProcessID;
		mapArgu.first = strExeFile;
		mapArgu.second = dwProcessID;
		stRTCOpenProcess.mapOpenProcess.insert(mapArgu);
	}
	refstRTCOpenProcess = stRTCOpenProcess;
	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CRealTimeCheck::CheckForeGround(ST_RTC_OPEN_PROCESS &refstRTCOpenProcess)
{
	HWND hforeground;
	hforeground = ::GetForegroundWindow();
	if (hforeground == NULL) {
		ErrorLog("Fail to get foregroundwindow");
		return E_RET_FAIL;
	}
	
	DWORD dwRet = E_RET_CHECK_FAIL;
	DWORD dwProcessId = 0;
	::GetWindowThreadProcessId(hforeground, &dwProcessId);
	std::map<std::string, DWORD>::iterator mapvecIter;
	for (mapvecIter = refstRTCOpenProcess.mapOpenProcess.begin(); mapvecIter != refstRTCOpenProcess.mapOpenProcess.end(); mapvecIter++) {
		if (!_stricmp(mapvecIter->first.c_str(), "iexplorer.exe") && mapvecIter->second == dwProcessId) {
			dwRet = E_RET_SUCCESS;
			break;
		}
		else if (!_stricmp(mapvecIter->first.c_str(), "chrome.exe") && mapvecIter->second == dwProcessId) {
			dwRet = E_RET_SUCCESS;
			break;
		}
	}
	return dwRet;
}

