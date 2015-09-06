#include "Common.h"

#include "..\..\XerosApp\XerosApp\HTMLParser\HTMLReader.h"

TEST(HTMLParser, HTMLPARSER_READER)
{
	CHTMLReader HTMLReader;
	std::string strHTMLData;
	std::string strFileName = "./test.html";
	DWORD dwRet;

	FILE *pFile = NULL;
	dwRet = fopen_s(&pFile, strFileName.c_str(), "r");
	ASSERT_TRUE(dwRet == 0);

	strHTMLData.clear();
	char szBuf[512] = { 0 };
	while (::fgets(szBuf, sizeof(szBuf), pFile))
	{
		strHTMLData += szBuf;
		::memset(szBuf, 0x00, sizeof(szBuf));
	}

	dwRet = HTMLReader.ParseHTML(strHTMLData);
	ASSERT_TRUE(dwRet == E_RET_SUCCESS);

	ST_HTML_URLS stHTMlURLs;
	dwRet = HTMLReader.GetURL(stHTMlURLs);
	ASSERT_TRUE(dwRet == E_RET_SUCCESS);

	::fclose(pFile);
}