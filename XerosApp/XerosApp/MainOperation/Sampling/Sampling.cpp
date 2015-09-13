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
		ST_DB_SQL stDBQuerySQL;
		stDBQuerySQL.strSQL = "SELECT sequence, frequency, word FROM \"WordRelation\" WHERE word = \'" + strValue + "\'";
		ST_DB_RESULT stDBResult;
		dwRet = QueryFromDB(m_hDataBase, stDBQuerySQL, stDBResult);
		if (dwRet != E_RET_SUCCESS) {
			ErrorLog("Fail to query [%s] from database", strValue);
			continue;
		}

		// word is not in database
		if (stDBResult.vecstrResult.size() < 1) {
			/* 
				because the word is not in database, word have to be initialized as default.
				default is 1

				Database example : stDBSQLInsert.strSQL = "INSERT INTO \"Dictionary\"(word) VALUES('" + strValue + "');
			*/
			ST_DB_SQL stDBInsertSQL;
			stDBInsertSQL.strSQL = "INSERT INTO \"WordRelation\"(sequence, frequency, word) VALUES(1, 1, \'" + strValue + "\')";
			dwRet = InsertToDB(m_hDataBase, stDBInsertSQL);
			if (dwRet != E_RET_SUCCESS) {
				ErrorLog("Fail to insert Value in DataBase");
				continue;
			}
		}
		// word is in database, so must update data in database
		else {
			std::vector<std::string>::iterator vecIter;
			for (vecIter = stDBResult.vecstrResult.begin(); vecIter != stDBResult.vecstrResult.end(); vecIter++) {
				std::string strRow = (*vecIter);
				DWORD dwFirstPos = 0, dwSecondPos = 0;

				// Value to get from database is word, sequence, frequency
				std::string strDBWord, strSequence, strFrequency;
				DWORD dwCount = 0;
				while (dwSecondPos < strRow.size()) {
					dwSecondPos = strRow.find_first_of(", ", dwFirstPos);
					if (dwSecondPos == std::string::npos) {
						strDBWord = strRow.substr(dwFirstPos, strRow.size() - dwFirstPos);
					}

					// if Count is 0, Value is sequence
					std::string strParam;
					if (dwCount == 0) {
						strParam = strRow.substr(dwFirstPos, dwSecondPos - dwFirstPos);
						DWORD dwValue = ::atoi(strParam.c_str()) + 1;
						strSequence = std::to_string(dwValue);
					}
					// if Count is 0, Value is frequency
					else if (dwCount == 1) {
						strParam = strRow.substr(dwFirstPos, dwSecondPos - dwFirstPos);
						DWORD dwValue = ::atoi(strParam.c_str()) + 1;
						strFrequency = std::to_string(dwValue);
					}

					// 2 is charater size (, )
					dwFirstPos = dwSecondPos + 2;
					dwCount++;
				}
				// each value update in database
				ST_DB_SQL stDBUpdateSQL;
				stDBUpdateSQL.strSQL = "UPDATE \"WordRelation\" SET sequence=" + strSequence + ", frequency=" + strFrequency + " WHERE word=\'" + strValue + "\'";
				dwRet = UpdateToDB(m_hDataBase, stDBUpdateSQL);
				if (dwRet != E_RET_SUCCESS) {
					ErrorLog("Fail to update Value in DataBase");
					continue;
				}
			}
		}
	}

	return E_RET_SUCCESS;
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

	/*
		Save string to DB
	*/
	DWORD dwRet;
	ST_DB_SQL stDBInsertSQL;
	stDBInsertSQL.strSQL = "INSERT INTO \"History\"(search) VALUES(\'" + refstrOutput + "\')";
	dwRet = InsertToDB(m_hDataBase, stDBInsertSQL);
	if (dwRet != E_RET_SUCCESS) {
		ErrorLog("Fail to insert Value in DataBase");
		dwRet = E_RET_FAIL;
	}
	return dwRet;
}

DWORD CSampling::CraftSentenceFromSearch(std::string &refstrSearchWord, std::string &refstrOutput)
{
	DWORD dwRet;
	try
	{
		/*
			sentence to get from key log data is separated by order
		*/
		std::vector<std::string> vecstrEnumWords;
		dwRet = EnumWordByOrder(refstrSearchWord, vecstrEnumWords);
		if (dwRet != 0) {
			throw std::exception("Fail to enum word by order");
		}

		/*
			word separated is not clear, because they have words not allowed (ex: in, a, ...)
			So, following method delete
		*/
		dwRet = SortoutWords(vecstrEnumWords);
		if (dwRet != 0) {
			throw std::exception("Fail to sort out word");
		}

		/*
			each word save to database -> relation word table
		*/
		dwRet = SupplementWorks(vecstrEnumWords);
		if (dwRet != 0) {
			throw std::exception("Fail to supplement works");
		}

		/*
			sentence save to database -> history table
		*/
		dwRet = SaveToString(vecstrEnumWords, refstrOutput);
		if (dwRet != 0) {
			throw std::exception("Fail to save to string");
		}
	}
	catch (std::exception &e)
	{
		printf("%s\n", e.what());
	}
	return E_RET_SUCCESS;
}
