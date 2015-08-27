#include "SearchWord.h"

CSearchWord::CSearchWord() : hDataBase(NULL)
{
}

CSearchWord::~CSearchWord()
{
}

DWORD CSearchWord::FindHistoryFromDatabase(std::string strInputWord, std::vector<std::string> &refvecstrHistory)
{
	ST_DB_SQL stDBSQLQuery;
	stDBSQLQuery.strSQL = "SELECT * FROM \"History\" where search LIKE \'%" + strInputWord +"\'%;";
	ST_DB_RESULT stDBResult;
	DWORD dwRet;
	dwRet = QueryFromDB(hDataBase, stDBSQLQuery, stDBResult);
	if (dwRet != 0) {
		return 0;
	}

	/*
		make list included input word
	*/
	std::vector<std::string> vecstrCraftResult;
	DWORD i;
	for (i = 0; i < stDBResult.vecstrResult.size(); i++) {
		std::string strValue = stDBResult.vecstrResult[i];
		vecstrCraftResult.push_back(strValue);
	}

	/*
		if result size is more than threshold, erase exceeded value
	*/
	DWORD dwNumberOfResult = vecstrCraftResult.size();
	if (dwNumberOfResult > MAX_SEARCH_HISTORY) {
		DWORD dwRemovedSize = dwNumberOfResult - MAX_SEARCH_HISTORY;
		std::vector<std::string>::iterator vecstrIter = vecstrCraftResult.begin();
		vecstrCraftResult.erase(vecstrIter, vecstrIter + dwRemovedSize - 1);
	}

	refvecstrHistory = vecstrCraftResult;
	return 0;
}

DWORD CSearchWord::EnumWords(std::string strInputWord, std::vector<std::string> &refvecstrEnumWord, std::vector<std::string> &refvecstrHistory)
{
	DWORD dwWordSize = strInputWord.size();
	std::vector<std::string>::iterator vecIter;
	for (vecIter = refvecstrHistory.begin(); vecIter != refvecstrHistory.end(); vecIter++) {
		std::string strResultString = (*vecIter);
		
		DWORD dwPreviousPos = 0, dwAfterPos = 0;
		while (dwPreviousPos >= strResultString.size()) {
			dwAfterPos = strResultString.find_first_of(" ", dwPreviousPos);
			if (dwAfterPos == std::string::npos) {
				std::string strExtractedWord = strResultString.substr(dwPreviousPos + 1, strResultString.size());
				if (strExtractedWord != strInputWord) {
					refvecstrEnumWord.push_back(strExtractedWord);
				}
				break;
			}
			
			std::string strExtractedWord = strResultString.substr(dwPreviousPos, dwAfterPos);
			if (strExtractedWord != strInputWord) {
				refvecstrEnumWord.push_back(strExtractedWord);
			}
			dwPreviousPos = dwAfterPos + 1;
		}
	}
	return 0;
}

DWORD CSearchWord::EnumDBWords(std::vector<std::string> &refvecstrEnumWord, std::vector<ST_WORD_DB_PROPERTY>& refvecstWordDBProperty)
{
	std::vector<std::string>::iterator vecIter;
	for (vecIter = refvecstrEnumWord.begin(); vecIter != refvecstrEnumWord.end(); vecIter++) {
		std::string strEnumString = (*vecIter);

		ST_DB_SQL stDBSQLQuery;
		stDBSQLQuery.strSQL = "SELECT * FROM \"WordRelation\" where word LIKE \'%" + strEnumString + "\'%;";
		ST_DB_RESULT stDBResult;
		DWORD dwRet;
		dwRet = QueryFromDB(hDataBase, stDBSQLQuery, stDBResult);
		if (dwRet != 0) {
			return 0;
		}

		/*
			Extract each data from result data
		*/
		if (stDBResult.vecstrResult.size() < 1)
			continue;

		std::vector<std::string>::iterator vecIterResult;
		for (vecIterResult = stDBResult.vecstrResult.begin(); vecIterResult != stDBResult.vecstrResult.end(); vecIterResult++) {
			std::string strResultString = (*vecIterResult);
			
			DWORD dwPreviousPos = 0, dwAfterPos = 0, dwTypeNumber = 0;
			ST_WORD_DB_PROPERTY stWordDBProperty;
			while (dwPreviousPos >= strResultString.size()) {
				dwAfterPos = strResultString.find_first_of(", ", dwPreviousPos);
				if (dwAfterPos == std::string::npos) {
					std::string strExtractedWord = strResultString.substr(dwPreviousPos + 1, strResultString.size());
					stWordDBProperty.dwFrequency = ::atoi(strExtractedWord.c_str());
					break;
				}

				std::string strExtractedWord = strResultString.substr(dwPreviousPos, dwAfterPos);
				if (dwTypeNumber == 0)
					stWordDBProperty.strWord = strExtractedWord;
				else if (dwTypeNumber == 1)
					stWordDBProperty.dwSequence = ::atoi(strExtractedWord.c_str());

				dwPreviousPos = dwAfterPos + 1;
			}
			refvecstWordDBProperty.push_back(stWordDBProperty);
		}
	}
	
}

DWORD CSearchWord::ChooseProperWords(std::vector<ST_WORD_DB_PROPERTY>& refvecstWordDBProperty, std::vector<std::string> &refvecstrResult)
{
	std::vector<ST_WORD_DB_PROPERTY>::iterator vecIterDBProperty;
	std::vector<std::string> vecstrFinalValue;
	ST_WORD_DB_PROPERTY stWordDBProperty;
	BOOL bFirst;
	for (vecIterDBProperty = refvecstWordDBProperty.begin(), bFirst = TRUE; vecIterDBProperty != refvecstWordDBProperty.end(); vecIterDBProperty++) {
		if (bFirst) {
			stWordDBProperty = (*vecIterDBProperty);
			bFirst = FALSE;
			continue;
		}
		if (stWordDBProperty.dwFrequency <= vecIterDBProperty->dwFrequency) {
			if (stWordDBProperty.dwFrequency != vecIterDBProperty->dwFrequency) {
				stWordDBProperty = (*vecIterDBProperty);
			}
			/*
				if frequency is same, check sequence
			*/
			else {
				if (stWordDBProperty.dwSequence < vecIterDBProperty->dwSequence) {
					stWordDBProperty = (*vecIterDBProperty);
				}
				else if (stWordDBProperty.dwSequence > vecIterDBProperty->dwSequence) {
					//nothing to do 
				}
				else {
					//nothing to do
				}
			}
		}
	}
	/*
		result size is only one
	*/
	refvecstrResult.push_back(stWordDBProperty.strWord);
	return 0;
}

DWORD CSearchWord::FindProperWords(std::string strInputWord, ST_PROPER_WORD_RET &refstProperWordRet)
{
	DWORD dwRet;
	try
	{
		std::vector<std::string> vecstrHistory;
		dwRet = FindHistoryFromDatabase(strInputWord, vecstrHistory);
		if (dwRet != 0) {
			throw std::exception("");
		}

		std::vector<std::string> vecstrEnumWord;
		dwRet = EnumWords(strInputWord, vecstrEnumWord, vecstrHistory);
		if (dwRet != 0) {
			throw std::exception("");
		}

		std::vector<ST_WORD_DB_PROPERTY> vecstWordDBProperty;
		dwRet = EnumDBWords(vecstrEnumWord, vecstWordDBProperty);
		if (dwRet != 0) {
			throw std::exception("");
		}

		/*
			below method is main module for extracting proper words
		*/
		std::vector<std::string> vecstrResult;
		dwRet = ChooseProperWords(vecstWordDBProperty, vecstrResult);
		if (dwRet != 0) {
			throw std::exception("");
		}
	}
	catch (std::exception &e)
	{
		printf("%s\n", e.what());
	}
	return 0;
}
