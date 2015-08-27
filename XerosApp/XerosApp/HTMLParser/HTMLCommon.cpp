#include "HTMLCommon.h"

void PassBlank(std::string &refstrHTMLData, DWORD &refdwPos)
{
	while (::isblank(refstrHTMLData[refdwPos]) && refdwPos < refstrHTMLData.size()) {
		refdwPos++;
	}
}

DWORD FindNextLetter(std::string &refstrHTMLData, DWORD dwPos, char cLetter)
{
	DWORD dwCurrentPos = dwPos;
	while (dwCurrentPos < refstrHTMLData.size()) {
		if (refstrHTMLData[dwCurrentPos] != cLetter) {
			dwCurrentPos++;
			continue;
		}
		break;
	}
	return dwCurrentPos;
}
