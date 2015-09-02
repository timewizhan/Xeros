#ifndef _BROWSER_
#define _BROWSER_

#include "..\Common\Common.h"
#include "..\Common\DataType.h"
#include "..\MainOperation\Network\Network.h"

class CBrowser
{
	ST_BROWSER	m_stBrowser;
public:
	CBrowser();
	~CBrowser();

	DWORD InitBrowser(E_BROWSER_TYPE eBrowserType);
	DWORD ExecuteBrowserWithArgs(ST_RESULT_URLS &refstResultUrls);
};


#endif