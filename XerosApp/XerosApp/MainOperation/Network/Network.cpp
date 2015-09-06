#include "Network.h"

///////////////////////////////////////////////////////////////////////////////////
CNetwork::CNetwork()
{
	::memset(&m_stNetwork, 0x00, sizeof(m_stNetwork));
}

///////////////////////////////////////////////////////////////////////////////////
CNetwork::~CNetwork()
{
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CNetwork::BuildHTTPReq(char *pBuf, const char *pKeyWord)
{
	/*
	Example
	URL : https://www.google.co.kr/webhp?hl=ko#newwindow=1&hl=ko&q=($word)

	GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1
	Host: net.tutsplus.com
	User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)
	Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*\/*; q = 0.8
	Accept - Language: en - us, en; q = 0.5
	Accept - Encoding: gzip, deflate
	Accept - Charset : ISO - 8859 - 1, utf - 8; q = 0.7, *; q = 0.7
	Keep - Alive: 300
	Connection : keep - alive
	Cookie : PHPSESSID = r2t5uvjq435r4q7ib3vtdjq120
	Pragma : no - cache
	Cache - Control : no - cache
	*/

	sprintf_s(pBuf, 512, "%s%s%s", "GET /search.nhn?query=", pKeyWord, "&kind=keyword HTTP/1.1\r\n"
		"Host: krdic.naver.com\r\n"
		"Connection: keep-alive\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) Applewebkit/537.36 (KHTML, like Gecko) Chrome/43.0.2357.81 Safari/537.36\r\n"
		"Accept-Encoding: gzip, deflate, sdch\r\n"
		"Accept-Language: ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4\r\n\r\n");

	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CNetwork::InitNetwork()
{
	int iRet;
	iRet = WSAStartup(MAKEWORD(2, 2), &m_stNetwork.wsaData);
	if (iRet != 0)
		return E_RET_FAIL;

	m_stNetwork.sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (m_stNetwork.sock == 0)
		return E_RET_FAIL;

	m_stNetwork.sockAddrIn.sin_family = AF_INET;
	m_stNetwork.sockAddrIn.sin_port = ::htons(CONNECT_TARGET_PORT);

	// 173.194.72.147 -> www.google.com IP address
	// use 'cmd' -> nslookup www.google.com
	std::string strTargetAddress = "173.194.72.147";
	::inet_pton(AF_INET, strTargetAddress.c_str(), &m_stNetwork.sockAddrIn.sin_addr.s_addr);

	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CNetwork::QueryFromNetwork(ST_SEARCH_REQ &refstSearchReq, ST_RECV_DATA &refstRecvData)
{
	DWORD dwError, dwErrorCode;
	dwError = ::connect(m_stNetwork.sock, (SOCKADDR *)&m_stNetwork.sockAddrIn, sizeof(SOCKADDR));
	if (dwError) {
		dwErrorCode = ::WSAGetLastError();
		ErrorLog("Fail to connect to URL [%d]", dwErrorCode);
		return E_RET_FAIL;
	}

	std::vector<std::string>::iterator vecIter;
	for (vecIter = refstSearchReq.vecstrStrings.begin(); vecIter != refstSearchReq.vecstrStrings.end(); vecIter++) {
#define HTTP_BUFFER_SIZE 512
		char szHTTP[HTTP_BUFFER_SIZE] = { 0 };
		dwError = BuildHTTPReq(szHTTP, (*vecIter).c_str());
		if (dwError != E_RET_SUCCESS) {
			ErrorLog("Fail to build http request [%s]", (*vecIter).c_str());
			continue;
		}

		DWORD dwSent;
		dwSent = ::send(m_stNetwork.sock, szHTTP, sizeof(szHTTP), 0);
		if (dwSent == SOCKET_ERROR) {
			DWORD dwSockError = ::WSAGetLastError();
			ErrorCode(dwSockError);
			continue;
		}

		dwError = ReceiveDataFromURL(refstRecvData);
		if (dwError != E_RET_SUCCESS) {
			ErrorLog("Fail to receive data from url");
			continue;
		}
	}

	DebugLog("Succeed to query from network");
	::closesocket(m_stNetwork.sock);
	return E_RET_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////
DWORD CNetwork::ReceiveDataFromURL(ST_RECV_DATA &refstRecvData)
{
	DWORD dwError;
	FD_SET readFD;
	timeval stTimeVal;
#define MAX_TIMEOUT_SEC		10
#define MAX_TIMEOUT_USEC	0
	stTimeVal.tv_sec	= MAX_TIMEOUT_SEC;
	stTimeVal.tv_usec	= MAX_TIMEOUT_USEC;

	BOOL bStart = TRUE;
	while (bStart) {
		FD_ZERO(&readFD);
		FD_SET(m_stNetwork.sock, &readFD);

		DWORD dwRet;
		dwRet = ::select(FD_SETSIZE, &readFD, 0, 0, &stTimeVal);
		if (dwRet == SOCKET_ERROR) {
			DWORD dwSockError = ::WSAGetLastError();
			ErrorCode(dwSockError);
			bStart = FALSE;
			continue;
		}
#define SELECT_TIMEOUT 0
		else if (dwRet == SELECT_TIMEOUT) {
			DebugLog("Select is timeover");
			bStart = FALSE;
			continue;
		}
		
		if (!FD_ISSET(m_stNetwork.sock, &readFD)) {
			ErrorLog("Not exist read fd set");
			bStart = FALSE;
			continue;
		}

		DWORD dwReceiveSize = 0;
		dwError = ::ioctlsocket(m_stNetwork.sock, FIONREAD, &dwReceiveSize);
		if (dwError == SOCKET_ERROR) {
			ErrorLog("Fail to get a read size");
			continue;
		}

		ST_RECV_DATA stRecvData;
		stRecvData.vecChar.resize(dwReceiveSize);
		stRecvData.dwTotalSize = dwReceiveSize;
		DWORD dwTotalRecv;
		dwTotalRecv = ::recv(m_stNetwork.sock, &stRecvData.vecChar[0], stRecvData.dwTotalSize, 0);
		if (dwTotalRecv <= 0) {
			bStart = FALSE;
			continue;
		}
		refstRecvData = stRecvData;
	}
	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
VOID CNetwork::ErrorCode(DWORD dwErrorCode)
{
	switch (dwErrorCode)
	{
		case WSANOTINITIALISED:
		case WSAEFAULT:
		case WSAENETDOWN:
		case WSAEINVAL:
		case WSAEINTR:
		case WSAEINPROGRESS:
		case WSAENOTSOCK:
			break;
		default:
			ErrorLog("Fail to use select method because of unknown error");
	}
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CNetwork::InitNetworkFromCURL()
{
	m_pCURL = curl_easy_init();
	if (m_pCURL == NULL) {
		return E_RET_FAIL;
	}

	curl_easy_setopt(m_pCURL, CURLOPT_HEADER, 0);
	curl_easy_setopt(m_pCURL, CURLOPT_VERBOSE, 0);
	curl_easy_setopt(m_pCURL, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(m_pCURL, CURLOPT_FILE, "./output");
	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CNetwork::QueryFromNetworkFromCURL(ST_SEARCH_REQ &refstSearchReq, ST_RECV_DATA &refstRecvData)
{
	std::vector<std::string>::iterator vecIter;
	for (vecIter = refstSearchReq.vecstrStrings.begin(); vecIter != refstSearchReq.vecstrStrings.end(); vecIter++) {
		std::string strSearchWord = (*vecIter);
		std::string strFullSearch = "http://www.google.com/search?q=" + strSearchWord;

		curl_easy_setopt(m_pCURL, CURLOPT_URL, strFullSearch.c_str());
		CURLcode CURLRet = curl_easy_perform(m_pCURL);
		if (CURLRet != CURLE_OK) {
			ErrorLog("Fail to search from CURL %s", curl_easy_strerror(CURLRet));
			continue;
		}

		double	statDouble;
		long	statLong;
		char	*statString;

		CURLRet = curl_easy_getinfo(m_pCURL, CURLINFO_HTTP_CODE, &statLong);
		if (CURLRet != CURLE_OK) {
			ErrorLog("Fail to get HTTP Code");
		}
		DebugLog("HTTP response code : %d", statLong);

		CURLRet = curl_easy_getinfo(m_pCURL, CURLINFO_CONTENT_TYPE, &statString);
		if (CURLRet != CURLE_OK) {
			ErrorLog("Fail to get Content Type");
		}
		DebugLog("Content Type : %s", statString);

		CURLRet = curl_easy_getinfo(m_pCURL, CURLINFO_SIZE_DOWNLOAD, &statDouble);
		if (CURLRet != CURLE_OK) {
			ErrorLog("Fail to get Size Download");
		}
		DebugLog("Download Size : %ld bytes", statDouble);

		CURLRet = curl_easy_getinfo(m_pCURL, CURLINFO_SPEED_DOWNLOAD, &statDouble);
		if (CURLRet != CURLE_OK) {
			ErrorLog("Fail to get Speed Download");
		}
		DebugLog("Download Speed : %ld bytes/sec", statDouble);

		DWORD dwRet;
		std::string strHTMLData;
		dwRet = ReadHTMLDataFromFile(strHTMLData);
		if (dwRet != E_RET_SUCCESS) {
			continue;
		}

		refstRecvData.strHTMLData = strHTMLData;
		refstRecvData.dwTotalSize = strHTMLData.size();
	}
	return E_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CNetwork::ReadHTMLDataFromFile(std::string &refstrHTMLData)
{
	FILE *pFile = NULL;
	DWORD dwRet;
	dwRet = fopen_s(&pFile, "./output", "r");
	if (dwRet != 0) {
		ErrorLog("Fail to open file");
		return E_RET_FAIL;
	}

	char szBuf[512] = { 0 };
	std::string strRealData;
	strRealData.clear();
	while (::fgets(szBuf, sizeof(szBuf), pFile))
	{
		strRealData += szBuf;
		::memset(szBuf, 0x00, sizeof(szBuf));
	}

	refstrHTMLData = strRealData;
	return E_RET_SUCCESS;
}