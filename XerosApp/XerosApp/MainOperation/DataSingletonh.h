#ifndef _DATA_SINGLETON_
#define _DATA_SINGLETON_

#include "..\Common\Common.h"
#include "..\Common\DataType.h"
#include "..\HTMLParser\HTMLStruct.h"

class CDataSingleton
{
	ST_RESULT_URLS			m_stResultURLs;
public:

	DWORD GetURLs(ST_RESULT_URLS &refstResultURLs);
	DWORD ChangeQueryTypeToBrowser(ST_HTML_URLS &refstHTMLURLs);

	static CDataSingleton* CDataSingleton::GetInstance();
};

static CDataSingleton*	m_pDataSingleton;

#endif