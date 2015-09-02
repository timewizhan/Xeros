#include "Common.h"

#include "..\..\XerosApp\XerosApp\Browser\Browser.h"

TEST(BROWSER, EXECUTE_BROWSER)
{
	CBrowser Browser;
	DWORD dwRet;
	dwRet = Browser.InitBrowser(E_BROWSER_IE);
	ASSERT_TRUE(dwRet == E_RET_SUCCESS);

	ST_RESULT_URLS stResultURLs;
	std::string strInsertURL1 = "www.google.com";
	std::string strInsertURL2 = "http://search.naver.com/search.naver?where=nexearch&query=test&sm=top_hty&fbm=0&ie=utf8";
	
	stResultURLs.vecstrURLs.push_back(strInsertURL1);
	stResultURLs.vecstrURLs.push_back(strInsertURL2);
	dwRet = Browser.ExecuteBrowserWithArgs(stResultURLs);
	ASSERT_TRUE(dwRet == E_RET_SUCCESS);
}