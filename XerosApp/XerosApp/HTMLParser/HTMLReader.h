#ifndef _HTML_READER_
#define _HTML_READER_

#include "HTMLSuper.h"
#include "HTMLTag.h"

class CHTMLReader
{
	ST_HTML_READER					m_stHTMLReader;
	std::vector<ST_HTML_TAG_DATA>	m_vecstHTMLTag;

	DWORD ReadHTMLData();
public:
	CHTMLReader();
	~CHTMLReader();

	DWORD ParseHTML(std::string &refstrHTMLData);
	DWORD GetURL(std::string &refstrURL);
};



#endif