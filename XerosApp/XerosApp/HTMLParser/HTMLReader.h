#ifndef _HTML_READER_
#define _HTML_READER_

#include "HTMLSuper.h"
#include "HTMLTag.h"

class CHTMLReader
{
	ST_HTML_READER					m_stHTMLReader;
	std::vector<ST_HTML_TAG_DATA>	m_vecstHTMLTag;

	DWORD ReadHTMLData();

	/*
		following method use only for google search
	*/
	DWORD ModifyURL(std::string &refstrUR);
public:
	CHTMLReader();
	~CHTMLReader();

	DWORD ParseHTML(std::string &refstrHTMLData);
	DWORD GetURL(ST_HTML_URLS &refstHTMLURL);
};



#endif