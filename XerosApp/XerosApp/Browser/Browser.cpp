#include "Browser.h"

///////////////////////////////////////////////////////////////////////////////////
CBrowser::CBrowser()
{
	::memset(&m_stBrowser, 0x00, sizeof(m_stBrowser));
}

///////////////////////////////////////////////////////////////////////////////////
CBrowser::~CBrowser()
{
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CBrowser::InitBrowser(E_BROWSER_TYPE eBrowserType)
{
	DWORD dwRet = E_RET_SUCCESS;
	m_stBrowser.eBrowserType = eBrowserType;
	switch (m_stBrowser.eBrowserType)
	{
		case E_BROWSER_IE:
			m_stBrowser.strFileName = "iexplore.exe";
			m_stBrowser.strFileFullPath = "C:\\Program Files\\Internet Explorer";
			break;
		case E_BROWSER_CHROME:
			m_stBrowser.strFileName = "chrome.exe";
			m_stBrowser.strFileFullPath = "C:\\Program Files\\Google\\Chrome\\Application";
			break;	
		default:
			ErrorLog("Fail to find Browser type");
			dwRet = E_RET_FAIL;
			break;
	}
	return dwRet;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CBrowser::ExecuteBrowserWithArg(std::string strURLs)
{
	std::string strFilePath = m_stBrowser.strFileFullPath + "\\" + m_stBrowser.strFileName;

	DWORD dwRet = E_RET_SUCCESS;
	LPCSTR pstrParam = strURLs.c_str();
	int nRet;
	nRet = (int)::ShellExecuteA(NULL, "open", strFilePath.c_str(), pstrParam, NULL, SW_SHOW);
	if (nRet < 32) {
		DWORD dwError = ::GetLastError();
		switch (dwError)
		{
		case 0:
		case ERROR_FILE_NOT_FOUND:
			ErrorLog("The system cannot find the file specified [%d]", dwError);
			break;
		case ERROR_PATH_NOT_FOUND:
			ErrorLog("The system cannot find the path specified [%d]", dwError);
			break;
		case ERROR_BAD_FORMAT:
			ErrorLog("An attempt was made to load a program with an incorrect format [%d]", dwError);
			break;
		case SE_ERR_ACCESSDENIED:
			ErrorLog("regular WinExec() code : access denied [%d]", dwError);
			break;
		case SE_ERR_ASSOCINCOMPLETE:
			ErrorLog("error values for ShellExecute() beyond the regular WinExec() codes [%d]", dwError);
			break;
		case SE_ERR_DDEBUSY:
			ErrorLog("error values for ShellExecute() beyond the regular WinExec() codes [%d]", dwError);
			break;
		case SE_ERR_DDEFAIL:
			ErrorLog("error values for ShellExecute() beyond the regular WinExec() codes [%d]", dwError);
			break;
		case SE_ERR_DDETIMEOUT:
			ErrorLog("error values for ShellExecute() beyond the regular WinExec() codes [%d]", dwError);
			break;
		case SE_ERR_DLLNOTFOUND:
			ErrorLog("regular WinExec() code : access denied [%d]", dwError);
			break;
		case SE_ERR_NOASSOC:
			ErrorLog("error values for ShellExecute() beyond the regular WinExec() codes [%d]", dwError);
			break;
		case SE_ERR_OOM:
			ErrorLog("regular WinExec() code : out of memory [%d]", dwError);
			break;
		case SE_ERR_SHARE:
			ErrorLog("error values for ShellExecute() beyond the regular WinExec() codes [%d]", dwError);
			break;
		default:
			ErrorLog("Unknown Error");
			break;
		}
		dwRet = E_RET_FAIL;
	}
	
	return dwRet;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CBrowser::ExecuteBrowserWithArgs(ST_RESULT_URLS &refstResultUrls)
{
	DWORD i, dwRet;
	for (i = 0; i < refstResultUrls.vecstrURLs.size(); i++) {
		std::string strURL = refstResultUrls.vecstrURLs[i];
		dwRet = ExecuteBrowserWithArg(strURL);
		if (dwRet != E_RET_SUCCESS) {
			ErrorLog("Fail to execute browser [%s]", strURL.c_str());
			continue;
		}
	}
	return dwRet;
}
