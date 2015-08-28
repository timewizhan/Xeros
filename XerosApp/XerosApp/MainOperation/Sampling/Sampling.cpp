#include "..\Sampling\Sampling.h"
#include "..\Sampling\SamplingType.h"

CSampling::CSampling(HANDLE hDataBase) : m_hDataBase(hDataBase)
{
}

CSampling::~CSampling()
{
}

DWORD CSampling::EnumWordByOrder(std::string &refstrSearchWord, std::vector<std::string> &refvecstrEnumWords)
{
	DWORD dwPreviousPos = 0, dwAfterPos = 0;
	DWORD dwSizeOfSearchWord = refstrSearchWord.size();
	std::vector<std::string> vecstrEnumWords;
	while (dwPreviousPos < dwSizeOfSearchWord) {
		std::string strValue;
		dwAfterPos = refstrSearchWord.find_first_of(" ", dwPreviousPos);
		if (dwAfterPos == std::string::npos) {
			strValue = refstrSearchWord.substr(dwPreviousPos, dwSizeOfSearchWord - dwPreviousPos);
			vecstrEnumWords.push_back(strValue);
			break;
		}
		strValue = refstrSearchWord.substr(dwPreviousPos, dwAfterPos - dwPreviousPos);
		vecstrEnumWords.push_back(strValue);
		dwPreviousPos = dwAfterPos + 1;
	}
	refvecstrEnumWords = vecstrEnumWords;
	return 0;
}

DWORD CSampling::SortoutWords(std::vector<std::string> &refvecstrEnumWords)
{
	DWORD dwVecEnumWordsSize = refvecstrEnumWords.size();
	DWORD dwArraySize = sizeof(RESTRICTION_WORDS) / sizeof(RESTRICTION_WORDS[0]);
	DWORD i, j;

	std::vector<std::string> strNewEnumArray;
	BOOL bFound = FALSE;
	for (i = 0; i < dwVecEnumWordsSize; i++) {
		std::string strEnumWord = refvecstrEnumWords[i];
		for (j = 0; j < dwArraySize; j++) {
#ifdef WIN32
			if (!_stricmp(strEnumWord.c_str(), RESTRICTION_WORDS[j])) {
#else
			if (!stricmp(strEnumWord.c_str(), RESTRICTION_WORDS[j])) {
#endif
				bFound = TRUE;
				break;
			}
			
		}
		/*
			After comparing two words, if value is 0 (e.g is same), insert the value to new EnumArray
		*/
		if (!bFound) {
			strNewEnumArray.push_back(strEnumWord);
		}
		bFound = FALSE;
	}
	refvecstrEnumWords.clear();
	refvecstrEnumWords = strNewEnumArray;
	
	return 0;
}

DWORD CSampling::SupplementWorks(std::vector<std::string> &refvecstrEnumWords)
{
	DWORD dwRet;
	std::vector<std::string>::iterator vecIterEnumWords;
	for (vecIterEnumWords = refvecstrEnumWords.begin(); vecIterEnumWords != refvecstrEnumWords.end(); vecIterEnumWords++) {
		std::string strValue = (*vecIterEnumWords);
		ST_DB_SQL stDBSQLInsert;
		stDBSQLInsert.strSQL = "INSERT INTO \"Dictionary\"(word) VALUES('" + strValue + "'); ";
		dwRet = InsertToDB(m_hDataBase, stDBSQLInsert);
		if (dwRet != 0)
			continue;
	}
	return 0;
}

DWORD CSampling::SaveToString(std::vector<std::string> &refvecstrEnumWords, std::string &refstrOutput)
{
	BOOL bStart = TRUE;
	std::vector<std::string>::iterator vecIter;
	for (vecIter = refvecstrEnumWords.begin(); vecIter != refvecstrEnumWords.end(); vecIter++) {
		if (bStart) {
			refstrOutput.clear();
			bStart = FALSE;
		}
		std::string strValue = (*vecIter);
		refstrOutput += strValue + " ";
	}
	// delete last character (blank)"
	refstrOutput.resize(refstrOutput.size() - 1);
	return 0;
}

DWORD CSampling::CraftSentenceFromSearch(std::string &refstrSearchWord, std::string &refstrOutput)
{
	DWORD dwRet;
	try
	{
		std::vector<std::string> vecstrEnumWords;
		dwRet = EnumWordByOrder(refstrSearchWord, vecstrEnumWords);
		if (dwRet != 0) {
			throw std::exception("");
		}

		dwRet = SortoutWords(vecstrEnumWords);
		if (dwRet != 0) {
			throw std::exception("");
		}

		/*dwRet = SupplementWorks(vecstrEnumWords);
		if (dwRet != 0) {
			throw std::exception("");
		}*/

		dwRet = SaveToString(vecstrEnumWords, refstrOutput);
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
