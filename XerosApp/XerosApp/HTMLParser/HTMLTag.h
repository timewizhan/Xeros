#ifndef _HTML_TAG_
#define _HTML_TAG_

#include "HTMLSuper.h"

class CHTMLTag
{
	ST_HTML_TAG	m_stHTMLTag;
	std::string	m_strHTMLData;
	DWORD		m_dwPos;

	DWORD CheckTag(E_HTML_TYPE eHTMLType);
	DWORD ParseHref(ST_HTML_TAG_DATA &refstHTMLTagData);
	DWORD ParseHrefContents(std::string &refstrhrefContents, DWORD dwLastPos);
public:
	CHTMLTag();
	~CHTMLTag();

	DWORD ParseTagData(std::string &refstrHTMLData, DWORD &refdwPos, ST_HTML_TAG_DATA &refstHTMLTagData);
};

#endif