#include "SearchWord.h"

CSearchWord::CSearchWord(HANDLE hDataBase) : m_hDataBase(hDataBase)
{
}

CSearchWord::~CSearchWord()
{
}

DWORD CSearchWord::FindHistoryFromDatabase(std::string strInputWord, std::vector<std::string> &refvecstrHistory)
{
	ST_DB_SQL stDBSQLQuery;
	stDBSQLQuery.strSQL = "SELECT * FROM \"History\" WHERE search LIKE \'%" + strInputWord +"%\';";
	ST_DB_RESULT stDBResult;
	DWORD dwRet;
	dwRet = QueryFromDB(m_hDataBase, stDBSQLQuery, stDBResult);
	if (dwRet != E_RET_SUCCESS) {
		return E_RET_FAIL;
	}

	if (stDBResult.vecstrResult.size() < 1) {
		DebugLog("Cannot find data from database");
		return E_RET_FAIL;
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
		vecstrCraftResult.erase(vecstrIter, vecstrIter + dwRemovedSize);
	}

	refvecstrHistory = vecstrCraftResult;
	return E_RET_SUCCESS;
}

DWORD CSearchWord::EnumWords(std::string strInputWord, std::vector<std::string> &refvecstrEnumWord, std::vector<std::string> &refvecstrHistory)
{
	std::vector<std::string> vecstrInputWord;
	DWORD dwFirstPos = 0, dwSecondPos = 0;
	while (dwSecondPos < strInputWord.size()) {
		dwSecondPos = strInputWord.find_first_of(" ", dwFirstPos);
		if (dwSecondPos == std::string::npos) {
			std::string strValue = strInputWord.substr(dwFirstPos, strInputWord.size() - dwFirstPos);
			vecstrInputWord.push_back(strValue);
			break;
		}
		std::string strValue = strInputWord.substr(dwFirstPos, dwSecondPos - dwFirstPos);
		vecstrInputWord.push_back(strValue);
		dwFirstPos = dwSecondPos + 1;
	}

	std::vector<std::string>::iterator vecIter;
	for (vecIter = refvecstrHistory.begin(); vecIter != refvecstrHistory.end(); vecIter++) {
		std::string strResultString = (*vecIter);
		
		DWORD dwPreviousPos = 0, dwAfterPos = 0;
		while (dwPreviousPos < strResultString.size()) {
			dwAfterPos = strResultString.find_first_of(" ", dwPreviousPos);
			if (dwAfterPos == std::string::npos) {
				std::string strExtractedWord = strResultString.substr(dwPreviousPos, strResultString.size() - dwPreviousPos);
				refvecstrEnumWord.push_back(strExtractedWord);
				break;
			}
			
			std::string strExtractedWord = strResultString.substr(dwPreviousPos, dwAfterPos - dwPreviousPos);
			refvecstrEnumWord.push_back(strExtractedWord);
			dwPreviousPos = dwAfterPos + 1;
		}
	}

	int i, j;
	for (i = 0; i < (int)vecstrInputWord.size(); i++) {
		for (j = 0; j < (int)refvecstrEnumWord.size(); j++) {
			if (vecstrInputWord[i] == refvecstrEnumWord[j]) {
				std::vector<std::string>::iterator vecIter;
				vecIter = refvecstrEnumWord.begin() + j;
				refvecstrEnumWord.erase(vecIter);
				j--;
			}
		}
	}



	if (refvecstrEnumWord.size() == 0) {
		DWORD dwi;
		for (dwi = 0; dwi < vecstrInputWord.size(); dwi++) {
			refvecstrEnumWord.push_back(vecstrInputWord[dwi]);
		}
	}
	
	return E_RET_SUCCESS;
}

DWORD CSearchWord::EnumDBWords(std::vector<std::string> &refvecstrEnumWord, std::vector<ST_WORD_DB_PROPERTY>& refvecstWordDBProperty)
{
	std::vector<std::string>::iterator vecIter;
	for (vecIter = refvecstrEnumWord.begin(); vecIter != refvecstrEnumWord.end(); vecIter++) {
		std::string strEnumString = (*vecIter);

		ST_DB_SQL stDBSQLQuery;
		stDBSQLQuery.strSQL = "SELECT * FROM \"WordRelation\" where word LIKE \'%" + strEnumString + "%\';";
		ST_DB_RESULT stDBResult;
		DWORD dwRet;
		dwRet = QueryFromDB(m_hDataBase, stDBSQLQuery, stDBResult);
		if (dwRet != E_RET_SUCCESS) {
			return E_RET_FAIL;
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
			while (dwPreviousPos < strResultString.size()) {
				dwAfterPos = strResultString.find_first_of(", ", dwPreviousPos);
				if (dwAfterPos == std::string::npos) {
					std::string strExtractedWord = strResultString.substr(dwPreviousPos, strResultString.size() - dwPreviousPos);
					stWordDBProperty.strWord = strExtractedWord;
					break;
				}

				std::string strExtractedWord = strResultString.substr(dwPreviousPos, dwAfterPos - dwPreviousPos);
				if (dwTypeNumber == 0)
					stWordDBProperty.dwSequence = ::atoi(strExtractedWord.c_str());
				else if (dwTypeNumber == 1)
					stWordDBProperty.dwFrequency = ::atoi(strExtractedWord.c_str());

				dwPreviousPos = dwAfterPos + 2;
				dwTypeNumber++;
			}
			refvecstWordDBProperty.push_back(stWordDBProperty);
		}
	}
	return E_RET_SUCCESS;
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
	return E_RET_SUCCESS;
}

DWORD CSearchWord::FindProperWords(std::string strInputWord, ST_PROPER_WORD_RET &refstProperWordRet)
{
	DWORD dwRet;
	try
	{
		/*
			First, find data from database
		*/
		std::vector<std::string> vecstrHistory;
		dwRet = FindHistoryFromDatabase(strInputWord, vecstrHistory);
		if (dwRet != E_RET_SUCCESS) {
			throw std::exception("Fail to find history from database");
		}

		/*
			Second, enum word from database except for word used for searching
		*/
		std::vector<std::string> vecstrEnumWord;
		dwRet = EnumWords(strInputWord, vecstrEnumWord, vecstrHistory);
		if (dwRet != E_RET_SUCCESS) {
			throw std::exception("Fail to enum word");
		}

		/*
			Third, get data from database.
			ex) Test, 1, 2
			Test is word
			1 is sequence
			2 is frequency
		*/
		std::vector<ST_WORD_DB_PROPERTY> vecstWordDBProperty;
		dwRet = EnumDBWords(vecstrEnumWord, vecstWordDBProperty);
		if (dwRet != E_RET_SUCCESS) {
			throw std::exception("Fail to enum db words");
		}

		/*
			below method is main module for extracting proper words
		*/
		std::vector<std::string> vecstrResult;
		dwRet = ChooseProperWords(vecstWordDBProperty, vecstrResult);
		if (dwRet != E_RET_SUCCESS) {
			throw std::exception("Fail to choose proper words");
		}

		refstProperWordRet.vecstrResult = vecstrResult;
	}
	catch (std::exception &e)
	{
		printf("%s\n", e.what());
	}
	return 0;
}
