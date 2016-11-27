#include "stdafx.h"
#include "globalvar.h"

namespace GlobalVar
{
	/*Thread handles*/
	HANDLE hReadThread;
	HANDLE hWriteThread;
	HANDLE hIdleWaitThread;
	HANDLE g_hIdleSendENQThread;

	HANDLE g_hEnqEvent;

	BOOL g_bWaitENQ = TRUE;
}