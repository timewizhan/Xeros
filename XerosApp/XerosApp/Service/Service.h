#ifndef _SERVICE_
#define _SERVICE_

#include "..\Common\Common.h"
#include <winsvc.h>

class CService
{
	SC_HANDLE m_hService;
	LPTSTR m_lpFilePath;
	LPTSTR m_lpServiceName;
	LPTSTR m_lpDisplayName;

	DWORD m_dwStartType;

	BOOL m_bInit;
	BOOL m_bLoaded;
	BOOL m_bStarted;

	inline BOOL  IsInit(void) {
		return m_bInit;
	}
	inline BOOL  IsLoaded(void) {
		return m_bLoaded;
	}
	inline BOOL  IsStarted(void) {
		return m_bStarted;
	}
public:
	CService(void);
	CService(LPTSTR lpFilePath, LPTSTR lpServiceName, LPTSTR lpDisplayName, DWORD dwStartType);
	~CService(void);

	DWORD InitSvc(LPTSTR lpFilePath, LPTSTR lpServiceName, LPTSTR lpDisplayName, DWORD dwStartType);
	DWORD CreateSvc(void);  
	DWORD UnloadSvc(void);  
	DWORD StartSvc(void);  
	DWORD StopSvc(void);   
};

#endif