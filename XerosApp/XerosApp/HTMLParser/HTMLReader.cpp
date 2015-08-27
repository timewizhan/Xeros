#include "HTMLReader.h"
#include "HTMLCommon.h"

///////////////////////////////////////////////////////////////////////////////////
CHTMLReader::CHTMLReader()
{
	::memset(&m_stHTMLReader, 0x00, sizeof(ST_HTML_READER));
}

///////////////////////////////////////////////////////////////////////////////////
CHTMLReader::~CHTMLReader()
{
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CHTMLReader::ReadHTMLData()
{
	/*
		Check blank.
		if you find blank, pass the position to next position
	*/
	PassBlank(m_stHTMLReader.strHTMLData, m_stHTMLReader.dwHTMLDataCurrentPos);

	CHTMLTag HTMLTag;
	while (m_stHTMLReader.dwHTMLDataCurrentPos < m_stHTMLReader.dwHTMLDataTotalSize) {
		char cLetter = m_stHTMLReader.strHTMLData[m_stHTMLReader.dwHTMLDataCurrentPos];
		if (cLetter == '<') {
			/*
				if you find '<' character, start to contruct CTag
			*/
			ST_HTML_TAG_DATA stHTMLTagData;
			DWORD dwRet;
			dwRet = HTMLTag.ParseTagData(m_stHTMLReader.strHTMLData, m_stHTMLReader.dwHTMLDataCurrentPos, stHTMLTagData);
			switch (dwRet)
			{
			case E_HTML_TAG_RET_START:
				m_vecstHTMLTag.push_back(stHTMLTagData);
				break;
			case E_HTML_TAG_RET_END:
				break;
			default:
				DebugLog("This position is not tag");
				break;
			}
		}
		/*
			Position of string should increment by 1
		*/
		//m_stHTMLReader.dwHTMLDataCurrentPos++;
		m_stHTMLReader.dwHTMLDataCurrentPos = FindNextLetter(m_stHTMLReader.strHTMLData, m_stHTMLReader.dwHTMLDataCurrentPos, '<');
		PassBlank(m_stHTMLReader.strHTMLData, m_stHTMLReader.dwHTMLDataCurrentPos);
	}
	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CHTMLReader::ParseHTML(std::string &refstrHTMLData)
{
	if (refstrHTMLData.size() <= 0) {
		ErrorLog("Cannot read HTML data : size[%d]", refstrHTMLData.size());
		return E_RET_FAIL;
	}

	m_stHTMLReader.strHTMLData			= refstrHTMLData;
	m_stHTMLReader.dwHTMLDataCurrentPos = 0;
	m_stHTMLReader.dwHTMLDataTotalSize	= refstrHTMLData.size();

	DWORD dwRet;
	dwRet = ReadHTMLData();
	if (dwRet != E_RET_SUCCESS) {
		ErrorLog("Fail to read HTML Data");
	}
	return dwRet;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CHTMLReader::GetURL(std::string &refstrURL)
{
	if (m_vecstHTMLTag.size() < 1) {
		return E_RET_FAIL;
	}

	std::vector<ST_HTML_TAG_DATA>::iterator			vecIter;
	std::map<std::string, std::string>::iterator	mapIter;
	for (vecIter = m_vecstHTMLTag.begin(); vecIter != m_vecstHTMLTag.end(); vecIter++) {
		for (mapIter = (*vecIter).mapTagData.begin(); mapIter != (*vecIter).mapTagData.end(); mapIter++) {
			refstrURL = mapIter->second;
		}
	}
	return E_RET_SUCCESS;
}