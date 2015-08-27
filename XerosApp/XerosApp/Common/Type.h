#ifndef _TYPE_
#define _TYPE_

enum E_RET_TYPE
{
	E_RET_SUCCESS = 0,

	/*
		Browser Return Type
	*/

	E_RET_SEARCH_FAIL,

	/*
		Analysis Return Type
	*/

	E_RET_CHECK_FAIL,

	/*
		etc...	
	*/

	E_RET_FAIL,
	E_RET_UNKNOWN
};

enum E_SCV_TYPE
{
	SVC_OK = 0,
	SVC_NOT_CREATE,
	SVC_NOT_START,
	SVC_NOT_INIT,
	SVC_ERROR_SCMANAGER,
	SVC_ERROR_CREATE,
	SVC_ERROR_START,
	SVC_ERROR_OPEN,
	SVC_ERROR_STOP,
	SVC_ERROR_UNLOAD,

	SVC_UNKNOWN
};


#endif