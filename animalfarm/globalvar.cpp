#include "stdafx.h"
#include "globalvar.h"

namespace GlobalVar
{
	/*Thread handles*/
	HANDLE g_hReadThread;
	HANDLE g_hWriteThread;
	HANDLE g_hIdleWaitThread;
	HANDLE g_hIdleSendENQThread;
	HANDLE g_hWaitConnectThread;
	HANDLE g_hWaitForACKThread;

	HANDLE g_hReceivingThread;
	HANDLE g_hReceivingWaitForACK;

	HANDLE g_hEnqEvent;
	HANDLE g_hRXSynEvent;
	HANDLE g_hAckEvent;

	BOOL g_bWaitENQ = TRUE;
	BOOL g_bWaitACK = TRUE;
	HANDLE g_hComm;

	COMMCONFIG g_cc;
}