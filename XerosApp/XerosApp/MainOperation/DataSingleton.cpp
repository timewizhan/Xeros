#include "DataSingletonh.h"

///////////////////////////////////////////////////////////////////////////////////
DWORD CDataSingleton::ChangeQueryTypeToBrowser(ST_HTML_URLS &refstHTMLURLs)
{
	if (m_stResultURLs.vecstrURLs.size() > 0) {
		m_stResultURLs.vecstrURLs.clear();
	}

	DWORD i;
	for (i = 0; refstHTMLURLs.vecstrHTMLURLs.size(); i++) {
		std::string strValue = refstHTMLURLs.vecstrHTMLURLs[i];
		m_stResultURLs.vecstrURLs.push_back(strValue);
	}

	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CDataSingleton::GetURLs(ST_RESULT_URLS &refstResultURLs)
{
	refstResultURLs = m_stResultURLs;
	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
CDataSingleton* CDataSingleton::GetInstance()
{
	if (m_pDataSingleton == NULL)
		return new CDataSingleton();
	return m_pDataSingleton;
}