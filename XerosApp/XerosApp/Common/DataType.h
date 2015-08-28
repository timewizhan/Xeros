#ifndef _DATA_TYPE_
#define _DATA_TYPE_

#include "Common.h"

enum E_BROWSER_TYPE
{
	E_BROWSER_IE = 0,
	E_BROWSER_CHROME,
	E_BROWSER_SAFARI,

	E_BROWSER_UNKNOWN
};


struct ST_BROWSER
{
	E_BROWSER_TYPE	eBrowserType;
	std::string		strFileName;
	std::string		strFileFullPath;
};

//struct ST_SEARCH_REQ
//{
//	std::vector<std::string> vecstrStrings;
//};

struct ST_ANALYSIS_RESULT
{
	std::vector<std::string> vecstrUrls;
};

//struct ST_RECV_DATA
//{
//	std::vector<char>	vecChar;
//	DWORD				dwTotalSize;
//
//	void operator=(ST_RECV_DATA &refstRecvData) {
//		this->vecChar = refstRecvData.vecChar;
//		this->dwTotalSize = refstRecvData.dwTotalSize;
//	}
//};

struct ST_RESULT_URLS
{
	std::vector<std::string> vecstrURLs;
};

#endif