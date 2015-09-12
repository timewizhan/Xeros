#include "Analyzer.h"

///////////////////////////////////////////////////////////////////////////////////
CAnalyzer::CAnalyzer(HANDLE hDataBase) :
m_pSampling(NULL),
m_pSearchWord(NULL),
m_hDataBase(hDataBase)
{
	assert(m_pSampling == NULL && m_pSearchWord == NULL);
	m_pSampling = new CSampling(m_hDataBase);
	if (m_pSampling == NULL) {
		ErrorLog("Fail to execute sampling");
	}

	m_pSearchWord = new CSearchWord();
	if (m_pSearchWord == NULL) {
		ErrorLog("Fail to exeucte search word");
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////////
CAnalyzer::~CAnalyzer()
{
	if (!m_pSampling)
		delete m_pSampling;
	if (!m_pSearchWord)
		delete m_pSearchWord;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CAnalyzer::StartAnalyzer(ST_PROPER_WORD_RET &refstProperWordRet)
{
	DWORD dwRet = E_RET_SUCCESS;
	try
	{
		std::string strSearchWord;
		dwRet = ReadKeyLoggerData(strSearchWord);
		if (dwRet != E_RET_SUCCESS) {
			throw std::exception("Fail to read key logger data");
		}

		std::string strFilterWord;
		dwRet = SamplingWord(strSearchWord, strFilterWord);
		if (dwRet != E_RET_SUCCESS) {
			throw std::exception("Fail to sample word");
		}

		ST_PROPER_WORD_RET stProperWordRet;
		dwRet = SearchWord(strFilterWord, stProperWordRet);
		if (dwRet != E_RET_SUCCESS) {
			throw std::exception("Fail to search word");
		}
		refstProperWordRet = stProperWordRet;
		/*
			below method use network for searhing selected word
		*/
		/*dwRet = QueryToWeb();
		if (dwRet != E_RET_SUCCESS) {
			throw std::exception("Fail to query to web");
		}*/
	}
	catch (std::exception &e)
	{
		ErrorLog("%s", e.what());
		dwRet = E_RET_FAIL;
	}
	return dwRet;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CAnalyzer::ReadKeyLoggerData(std::string &refstrSearchWord)
{
	FILE *pFile = NULL;

	LPWSTR lpwBuffer = NULL;
	GetCurrentDirectory(512, lpwBuffer);
	std::wstring strBuffer = lpwBuffer;
	std::wstring strFileName = DRIVER_OUTPUT_FILE_NAME;
	strBuffer += L"/" + strFileName;

	/*
		check whether file exist on file path indicated	
	*/
	BOOL bRet;
	bRet = PathFileExists(strBuffer.c_str());
	if (!bRet) {
		return E_RET_FAIL;
	}

	std::string strFileFullPath;
	strFileFullPath.assign(strBuffer.begin(), strBuffer.end());
#ifdef WIN32
	
	errno_t errRet;
	errRet = ::fopen_s(&pFile, strFileFullPath.c_str(), "r");
	if (errRet != 0) {
		return E_RET_FAIL;
	}
#else
	pFile = ::fopen(FILE_LOGGER_PATH, "r");
#endif
	if (!pFile) {
		ErrorLog("Fail to get file pointer [%s]", FILE_LOGGER_PATH);
	}

#define MAX_BUFFER 512
	char szBuf[MAX_BUFFER] = { 0 };
	std::vector<std::string> vecstrFileData;
	while (fgets(szBuf, MAX_BUFFER, pFile)) {
		std::string strValue = szBuf;
		vecstrFileData.push_back(strValue);
		::memset(szBuf, 0x00, sizeof(szBuf));
	}

	std::string strSearchWord;
	if (vecstrFileData.size() > 1) {
		strSearchWord = vecstrFileData[vecstrFileData.size() - 1];
	}
	else {
		/*
			because vector has only one
		*/
		strSearchWord = vecstrFileData[0];
	}
	refstrSearchWord = strSearchWord;
	::fclose(pFile);
	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CAnalyzer::SamplingWord(std::string &refstrSearchWord, std::string &refstrFilterWord)
{
	return m_pSampling->CraftSentenceFromSearch(refstrSearchWord, refstrFilterWord);
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CAnalyzer::SearchWord(std::string &refstrFilterWord, ST_PROPER_WORD_RET &refstProperWordRet)
{
	return m_pSearchWord->FindProperWords(refstrFilterWord, refstProperWordRet);
}

///////////////////////////////////////////////////////////////////////////////////
//DWORD CAnalyzer::QueryToWeb(ST_PROPER_WORD_RET &refstProperWordRet)
//{
//	std::vector<std::string>::iterator vecstrIterProper;
//	ST_SEARCH_REQ stSearchReq;
//	for (vecstrIterProper = refstProperWordRet.vecstrResult.begin(); vecstrIterProper != refstProperWordRet.vecstrResult.end(); vecstrIterProper++) {
//		std::string strValue = (*vecstrIterProper);
//		stSearchReq.vecstrStrings.push_back(strValue);
//	}
//
//	if (stSearchReq.vecstrStrings.size() < 1) {
//		ErrorLog("Search words have none");
//		return E_RET_FAIL;
//	}
//	
//	DWORD dwRet;
//	ST_RECV_DATA stRecvData;
//	dwRet = m_pNetwork->QueryFromNetwork(stSearchReq, stRecvData);
//	if (dwRet != E_RET_SUCCESS) {
//		ErrorLog("Fail to query network using search word");
//		return dwRet;
//	}
//
//	std::string strHTMLString;
//	strHTMLString.resize(stRecvData.dwTotalSize);
//	DWORD i;
//	for (i = 0; i < stRecvData.vecChar.size(); i++) {
//		strHTMLString = stRecvData.vecChar[i];
//	}
//
//	CHTMLReader HTMLReader;
//	dwRet = HTMLReader.ParseHTML(strHTMLString);
//	if (dwRet != E_RET_SUCCESS) {
//		ErrorLog("Fail to parse HTML String [%d]", stRecvData.dwTotalSize);
//		return dwRet;
//	}
//	dwRet = HTMLReader.GetURL(m_strURL);
//	if (dwRet != E_RET_SUCCESS) {
//		ErrorLog("Fail to get URL");
//		return dwRet;
//	}
//	return dwRet;
//}
//
