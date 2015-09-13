#ifndef _NETWORK_
#define _NETWORK_

#include "..\..\Common\Common.h"
#include "NetworkType.h"

#define _CURL_
#ifdef _CURL_
#include "curl\curl.h"
#endif

#define CONNECT_TARGET_URL	"www.google.com"
#define CONNECT_TARGET_PORT	 80

class CNetwork
{
	ST_NETWORK m_stNetwork;

	CURL		*m_pCURL;
	FILE		*m_pFile;
	std::string	strFileName;
	std::string		m_strFilePath;

	DWORD BuildHTTPReq(char *pBuf, const char *pKeyWord);
	DWORD ReceiveDataFromURL(ST_RECV_DATA &refstRecvData);
	VOID ErrorCode(DWORD dwErrorCode);

	/*
		Only for CURL
	*/
	DWORD ReadHTMLDataFromFile(std::string &refstrHTMLData);
	DWORD InitNetworkFromCURL();
public:
	CNetwork();
	~CNetwork();

	DWORD InitNetwork();
	DWORD QueryFromNetwork(ST_SEARCH_REQ &refstSearchReq, ST_RECV_DATA &refstRecvData);

	/*
		following method is used for only cURL
	*/
	DWORD QueryFromNetworkFromCURL(ST_SEARCH_REQ &refstSearchReq, ST_RECV_DATA &refstRecvData);
};

#endif