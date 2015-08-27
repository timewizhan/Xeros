#include "Service.h"

///////////////////////////////////////////////////////////////////////////////////
CService::CService(void) : 
	m_lpFilePath(NULL),
	m_lpServiceName(NULL),
	m_lpDisplayName(NULL),
	m_dwStartType(0),
	m_hService(NULL),
	m_bInit(FALSE),
	m_bLoaded(FALSE),
	m_bStarted(FALSE)
{
}

///////////////////////////////////////////////////////////////////////////////////
CService::CService(LPTSTR lpFilePath, LPTSTR lpServiceName, LPTSTR lpDisplayName, DWORD dwStartType) :
	m_lpFilePath(lpFilePath),
	m_lpServiceName(lpServiceName),
	m_lpDisplayName(lpDisplayName),
	m_dwStartType(dwStartType),
	m_hService(NULL),
	m_bInit(TRUE),
	m_bLoaded(FALSE),
	m_bStarted(FALSE)
{
}

///////////////////////////////////////////////////////////////////////////////////
CService::~CService(void)
{
	DWORD dwRet;
	dwRet = UnloadSvc();
	if (dwRet != SVC_OK) {
		ErrorLog("Fail to close service");
	}
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CService::InitSvc(LPTSTR lpFilePath, LPTSTR lpServiceName, LPTSTR lpDisplayName, DWORD dwStartType)
{
	if (IsInit())
		return SVC_NOT_INIT;

	m_lpFilePath = lpFilePath;
	m_lpServiceName = lpServiceName;
	m_lpDisplayName = lpDisplayName;
	m_dwStartType = dwStartType;

	m_hService = NULL;

	m_bInit = TRUE;
	m_bLoaded = FALSE;
	m_bStarted = FALSE;

	return SVC_OK;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CService::CreateSvc(void)
{
	if (!IsInit())
		return SVC_NOT_INIT;

	if (IsLoaded())
		return SVC_OK;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	DWORD dwError = ::GetLastError();
	if (hSCManager == NULL) {
		ErrorLog("Fail to create service [%d]", dwError);
		return SVC_ERROR_SCMANAGER;
	}

	m_hService = CreateService(hSCManager, m_lpServiceName, m_lpDisplayName,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		m_dwStartType,
		SERVICE_ERROR_NORMAL,
		m_lpFilePath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	dwError = ::GetLastError();
	if (m_hService == NULL){
		m_hService = OpenService(hSCManager, m_lpServiceName, SERVICE_ALL_ACCESS);
		if (m_hService == NULL) {
			CloseServiceHandle(hSCManager);
			return SVC_ERROR_CREATE;
		}
	}
	m_bLoaded = TRUE;
	CloseServiceHandle(hSCManager);
	DebugLog("Start to create service");
	return SVC_OK;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CService::StartSvc(void)
{
	if (!IsLoaded())
		return SVC_NOT_CREATE;

	if (IsStarted())
		return SVC_OK;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
		return SVC_ERROR_SCMANAGER;

	m_hService = OpenService(hSCManager, m_lpServiceName, SERVICE_ALL_ACCESS);
	if (m_hService == NULL) {
		ErrorLog("Not exist service handle");
		CloseServiceHandle(hSCManager);
		return SVC_ERROR_OPEN;
	}

	BOOL bRet;
	bRet = StartService(m_hService, 0, NULL);
	DWORD dwError = ::GetLastError();
	if (bRet == FALSE){
		ErrorLog("Fail to start service [%d]", dwError);
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(m_hService);
		return SVC_ERROR_START;
	}
	CloseServiceHandle(hSCManager);
	m_bStarted = TRUE;
	return SVC_OK;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CService::StopSvc(void)
{
	SERVICE_STATUS ss;

	if (!IsStarted())
		return SVC_OK;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (hSCManager == NULL)
		return SVC_ERROR_SCMANAGER;

	m_hService = OpenService(hSCManager, m_lpServiceName, SERVICE_ALL_ACCESS);
	if (m_hService == NULL) {
		CloseServiceHandle(hSCManager);
		return SVC_ERROR_OPEN;
	}

	if (ControlService(m_hService, SERVICE_CONTROL_STOP, &ss) == NULL){
		ErrorLog("Fail to stop service");
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(m_hService);
		return SVC_ERROR_STOP;
	}

	m_bStarted = false;
	CloseServiceHandle(hSCManager);
	CloseServiceHandle(m_hService);
	return SVC_OK;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CService::UnloadSvc(void)
{
	if (!IsLoaded())
		return SVC_OK;

	if (IsStarted())
		if (StopSvc() != SVC_OK)
			return SVC_ERROR_UNLOAD;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hSCManager == NULL)
		return SVC_ERROR_SCMANAGER;

	m_hService = OpenService(hSCManager, m_lpServiceName, SERVICE_ALL_ACCESS);
	if (m_hService == NULL) {
		CloseServiceHandle(hSCManager);
		return SVC_ERROR_OPEN;
	}

	m_bLoaded = false;
	DeleteService(m_hService);
	CloseServiceHandle(hSCManager);
	return SVC_OK;
}

