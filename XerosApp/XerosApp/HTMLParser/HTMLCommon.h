#ifndef _HTML_COMMON_
#define _HTML_COMMON_

#include "HTMLSuper.h"

void PassBlank(std::string &refstrHTMLData, DWORD &refdwPos);
DWORD FindNextLetter(std::string &refstrHTMLData, DWORD dwPos, char cLetter);

#endif