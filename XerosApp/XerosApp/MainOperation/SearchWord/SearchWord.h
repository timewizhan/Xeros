#ifndef _SEARCH_WORD_
#define _SEARCH_WORD_

#include "SearchWordType.h"
#include "..\..\DataBase\DataBase.h"

#define MAX_SEARCH_HISTORY 5

class CSearchWord
{
	HANDLE hDataBase;

	DWORD FindHistoryFromDatabase(std::string strInputWord, std::vector<std::string> &refvecstrHistory);
	DWORD EnumWords(std::string strInputWord, std::vector<std::string> &refvecstrEnumWord, std::vector<std::string> &refvecstrHistory);
	DWORD EnumDBWords(std::vector<std::string> &refvecstrEnumWord, std::vector<ST_WORD_DB_PROPERTY>& refvecstWordDBProperty);
	DWORD ChooseProperWords(std::vector<ST_WORD_DB_PROPERTY>& refvecstWordDBProperty, std::vector<std::string> &refvecstrResult);
public:
	CSearchWord();
	~CSearchWord();

	DWORD FindProperWords(std::string strInputWord, ST_PROPER_WORD_RET &refstProperWordRet);
};


#endif