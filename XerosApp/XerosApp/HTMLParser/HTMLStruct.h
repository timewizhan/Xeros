#ifndef _HTML_STRUCT_
#define _HTML_STRUCT_

#include "..\Common\Common.h"
#include "HTMLType.h"

struct ST_HTML_READER
{
	std::string	strHTMLData;
	DWORD		dwHTMLDataCurrentPos;
	DWORD		dwHTMLDataTotalSize;
};

struct ST_HTML_TAG_DATA
{
	E_HTML_TYPE							eHTMLType;
	std::map<std::string, std::string>	mapTagData;
};


/*
	if you start to check character in html..
	<html>  <-- bTagStart = TRUE
	</html> <-- bTagEnd = TRUE
*/

struct ST_HTML_TAG_START_END
{
	BOOL		bTagStart;
	BOOL		bTagEnd;
};

/*
	if you start to read '<' character in html..
	'<' <-- bSignStart = TRUE
	'>' <-- bSignEnd = TRUE
*/

struct ST_HTML_TAG_SIGN
{
	BOOL		bSignStart;
	BOOL		bSignEnd;
};

struct ST_HTML_TAG
{
	E_HTML_TYPE						eHTMLType;
	ST_HTML_TAG_START_END			stHTMLTagStartEnd;
	std::vector<ST_HTML_TAG_SIGN>	vecstHTMLTagSign;
};

#endif