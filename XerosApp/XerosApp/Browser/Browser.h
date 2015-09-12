#ifndef _BROWSER_
#define _BROWSER_

#include "..\Common\Common.h"
#include "..\Common\DataType.h"
#include "..\HTMLParser\HTMLStruct.h"

class CBrowser
{
	ST_BROWSER			m_stBrowser;
	ST_RESULT_URLS		m_stResultURLs;
	
	static CBrowser		*m_pBrowser;
public:
	CBrowser();
	~CBrowser();

	DWORD InitBrowser(E_BROWSER_TYPE eBrowserType);

	DWORD ExecuteBrowserWithArg(std::string strURLs);
	DWORD ExecuteBrowserWithArgs(ST_RESULT_URLS &refstResultUrls);

	/*
		Browser class is singleton instance
	*/
	inline static CBrowser* GetBrowserInstance() {
		return NULL;
	}
};

#endif