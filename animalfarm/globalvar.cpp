#include "stdafx.h"
#include "globalvar.h"

namespace GlobalVar
{
	/*Thread handles*/
	HANDLE g_hReadThread;
	HANDLE g_hWriteThread;
	HANDLE g_hIdleWaitThread;
	HANDLE g_hIdleSendENQThread;

	HANDLE g_hReceivingThread;

	HANDLE g_hEnqEvent;

	BOOL g_bWaitENQ = TRUE;
}