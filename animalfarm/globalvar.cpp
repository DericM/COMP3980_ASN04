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
	HANDLE g_hReceivingWaitForACK;

	HANDLE g_hEnqEvent;
	HANDLE g_hRXSynEvent;

	BOOL g_bWaitENQ = TRUE;
	HANDLE g_hComm;
}