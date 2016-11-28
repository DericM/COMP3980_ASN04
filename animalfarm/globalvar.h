#pragma once

namespace GlobalVar
{
	/*Thread handles*/
	extern HANDLE g_hReadThread;
	extern HANDLE g_hWriteThread;
	extern HANDLE g_hIdleWaitThread;
	extern HANDLE g_hIdleSendENQThread;

	extern HANDLE g_hReceivingThread;
	extern HANDLE g_hReceivingWaitForACK;

	extern HANDLE g_hEnqEvent;

	extern BOOL g_bWaitENQ;
	extern HANDLE g_hComm;
}