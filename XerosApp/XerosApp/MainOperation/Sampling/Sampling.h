#ifndef _SAMPLING_
#define _SAMPLING_

#include "..\..\Common\Common.h"
#include "..\..\DataBase\DataBase.h"

class CSampling
{
	HANDLE m_hDataBase;

	DWORD EnumWordByOrder(std::string &refstrSearchWord, std::vector<std::string> &refvecstrEnumWords);
	DWORD SortoutWords(std::vector<std::string> &refvecstrEnumWords);
	DWORD SupplementWorks(std::vector<std::string> &refvecstrEnumWords);
	DWORD SaveToString(std::vector<std::string> &refvecstrEnumWords, std::string &refstrOutput);
public:
	CSampling(HANDLE hDataBase);
	~CSampling();

	DWORD CraftSentenceFromSearch(std::string &refstrSearchWord, std::string &refstrOutput);
};

#endif