#ifndef _NETWORK_TYPE_
#define _NETWORK_TYPE_

struct ST_NETWORK
{
	WSADATA		wsaData;
	SOCKET		sock;
	SOCKADDR_IN sockAddrIn;
};

struct ST_SEARCH_REQ
{
	std::vector<std::string> vecstrStrings;
};

struct ST_RECV_DATA
{
	std::vector<char>	vecChar;
	std::string			strHTMLData;
	DWORD				dwTotalSize;

	void operator=(ST_RECV_DATA &refstRecvData) {
		this->vecChar = refstRecvData.vecChar;
		this->dwTotalSize = refstRecvData.dwTotalSize;
	}
};


#endif