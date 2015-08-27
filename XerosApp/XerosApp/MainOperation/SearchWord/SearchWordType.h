#ifndef _SEARCH_WORD_TYPE_
#define _SEARCH_WORD_TYPE_

#include "..\..\Common\Common.h"

struct ST_PROPER_WORD_RET
{
	std::vector<std::string> vecstrResult;
};

struct ST_WORD_DB_PROPERTY
{
	std::string strWord;
	DWORD		dwSequence;
	DWORD		dwFrequency;
};

#endif