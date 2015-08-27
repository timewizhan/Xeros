#include "HTMLTag.h"
#include "HTMLCommon.h"

///////////////////////////////////////////////////////////////////////////////////
CHTMLTag::CHTMLTag()
{
	::memset(&m_stHTMLTag, 0x00, sizeof(ST_HTML_TAG));
}

///////////////////////////////////////////////////////////////////////////////////
CHTMLTag::~CHTMLTag()
{
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CHTMLTag::ParseHrefContents(std::string &refstrhrefContents, DWORD dwLastPos)
{
	DWORD dwCommaStart = 0, dwCommaEnd = 0;
	BOOL bCommaStart = FALSE, bCommaEnd = FALSE;
	while (m_dwPos++ < dwLastPos) {
		char cLetter = m_strHTMLData[m_dwPos];
		if (cLetter != '\"')
			continue;

		if (bCommaStart == FALSE) {
			bCommaStart = TRUE;
			dwCommaStart = m_dwPos;
			continue;
		}
		if (bCommaStart == TRUE) {
			bCommaEnd = TRUE;
			dwCommaEnd = m_dwPos;
			break;
		}
	}
	if (!bCommaStart || !bCommaEnd) {
		return E_RET_FAIL;
	}

	std::string strContents = m_strHTMLData;
	refstrhrefContents = strContents.substr(dwCommaStart + 1, dwCommaEnd - dwCommaStart - 1);
	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CHTMLTag::ParseHref(ST_HTML_TAG_DATA &refstHTMLTagData)
{
	DWORD dwRet = E_RET_SUCCESS;
	DWORD dwFirstTagPos = m_dwPos;
	DWORD dwLastTagPos = FindNextLetter(m_strHTMLData, m_dwPos, '>');

	BOOL bContinue = TRUE;
	while (bContinue && dwFirstTagPos++ < dwLastTagPos) {
		PassBlank(m_strHTMLData, dwFirstTagPos);
		char cLetter = m_strHTMLData[dwFirstTagPos];
		/*
			find attribute 
			there is an example => href="www.google.com"
		*/
		if (strncmp(&m_strHTMLData[dwFirstTagPos], "href", 4) != 0)
			continue;

		dwFirstTagPos += 4;
		cLetter = m_strHTMLData[dwFirstTagPos];
		if (cLetter != '=')
			continue;

		std::string strhrefContents;
		m_dwPos = dwFirstTagPos;
		dwRet = ParseHrefContents(strhrefContents, dwLastTagPos);
		if (dwRet != E_RET_SUCCESS) {
			ErrorLog("Fail to parse href contents data");
		}
		refstHTMLTagData.eHTMLType = E_HTML_a;
		std::pair<std::string, std::string> pairTagData("href", strhrefContents);
		refstHTMLTagData.mapTagData.insert(pairTagData);

		bContinue = FALSE;
	}
	return dwRet;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CHTMLTag::ParseTagData(std::string &refstrHTMLData, DWORD &refdwPos, ST_HTML_TAG_DATA &refstHTMLTagData)
{
	DWORD dwRet;
	m_strHTMLData = refstrHTMLData;
	/*
		refdwPos indicate position of '<'
		so you should use + 1 for next position
	*/
	m_dwPos = refdwPos + 1;
	while (1) {
		PassBlank(m_strHTMLData, m_dwPos);
		char cLetter = m_strHTMLData[m_dwPos];
		if (cLetter == '/') {
			/*
				We should consider that '/' letter is end of tag
			*/
			m_dwPos++;
			PassBlank(m_strHTMLData, m_dwPos);
	
			m_dwPos = FindNextLetter(m_strHTMLData, m_dwPos, '>');
			dwRet = E_HTML_TAG_RET_END;
			break;
		}
		else {
			m_stHTMLTag.stHTMLTagStartEnd.bTagStart = TRUE;
			PassBlank(m_strHTMLData, m_dwPos);

			dwRet = CheckTag(E_HTML_a);
			if (dwRet == E_HTML_TAG_FOUND) {
				dwRet = ParseHref(refstHTMLTagData);
				if (dwRet != E_RET_SUCCESS) {
					dwRet = E_HTML_TAG_PARSE_FAIL;
				}
				else {
					dwRet = E_HTML_TAG_RET_START;
				}
				m_dwPos = FindNextLetter(m_strHTMLData, m_dwPos, '>');
			}
			else {
				//	Only find 'a' tag
				m_dwPos = FindNextLetter(m_strHTMLData, m_dwPos, '>');
			}
			break;
		}
		m_dwPos++;
	}
	refdwPos = m_dwPos;
	return dwRet;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CHTMLTag::CheckTag(E_HTML_TYPE eHTMLType)
{
	DWORD dwRet = E_HTML_TAG_NOT_FOUND;
	switch (eHTMLType)
	{
	case E_HTML_COMMENT:
		break;
	case E_HTML_DOCTYPE:
		break;
	case E_HTML_a:
		if (!strncmp(&m_strHTMLData[m_dwPos], "a", 1)) {
			m_dwPos++;
			dwRet = E_HTML_TAG_FOUND;
		}
		break;
	default:
		break;
	}
	return dwRet;
}