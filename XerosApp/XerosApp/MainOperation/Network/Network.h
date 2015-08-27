#ifndef _NETWORK_
#define _NETWORK_

#include "..\..\Common\Common.h"
#include "NetworkType.h"

#define CONNECT_TARGET_URL	"www.google.com"
#define CONNECT_TARGET_PORT	 80

class CNetwork
{
	ST_NETWORK m_stNetwork;

	DWORD BuildHTTPReq(char *pBuf, const char *pKeyWord);
	DWORD ReceiveDataFromURL(ST_RECV_DATA &refstRecvData);
	VOID ErrorCode(DWORD dwErrorCode);
public:
	CNetwork();
	~CNetwork();

	DWORD InitNetwork();
	DWORD QueryFromNetwork(ST_SEARCH_REQ &refstSearchReq, ST_RECV_DATA &refstRecvData);
};

#endif