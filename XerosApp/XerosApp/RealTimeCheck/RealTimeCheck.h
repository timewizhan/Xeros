#ifndef _REALTIME_CHECK_
#define _REALTIME_CHECK_

#include "Common\Common.h"
#include "RealTimeCheckType.h"

#define SLEEP_TIME 1000

class CRealTimeCheck
{
	DWORD GetCurrentOpenProcess(ST_RTC_OPEN_PROCESS &refstRTCOpenProcess);
	DWORD CheckForeGround(ST_RTC_OPEN_PROCESS &refstRTCOpenProcess);
public:
	CRealTimeCheck();
	~CRealTimeCheck();

	DWORD StartRealTimeCheck();
};

#endif