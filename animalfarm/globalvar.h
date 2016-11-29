#pragma once

namespace GlobalVar
{
	/*Thread handles*/
	extern HANDLE g_hReadThread;
	extern HANDLE g_hWriteThread;
	extern HANDLE g_hIdleWaitThread;
	extern HANDLE g_hIdleSendENQThread;
	extern HANDLE g_hWaitConnectThread;
	extern HANDLE g_hWaitForACKThread;

	extern HANDLE g_hReceivingThread;
	extern HANDLE g_hReceivingWaitForACK;

	extern HANDLE g_hEnqEvent;
	extern HANDLE g_hAckEvent;
	extern HANDLE g_hRXSynEvent;

	extern BOOL g_bWaitENQ;
	extern BOOL g_bWaitACK;
	extern HANDLE g_hComm;

	extern COMMCONFIG g_cc;
}