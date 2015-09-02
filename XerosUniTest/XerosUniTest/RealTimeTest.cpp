#include "Common.h"

#include "..\..\XerosApp\XerosApp\RealTimeCheck\RealTimeCheck.h"

TEST(REALTIMECHECK, PROCESS_CHECK)
{
	CRealTimeCheck RealTimeCheck;
	DWORD dwRet;
	while (1) {
		dwRet = RealTimeCheck.StartRealTimeCheck();
		if (dwRet == E_RET_CHECK_FAIL) {
			::Sleep(1000);
			continue;
		}
		else {
			ASSERT_TRUE(dwRet == E_RET_SUCCESS);
			break;
		}
	}
}