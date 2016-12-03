#pragma once

namespace GlobalVar
{
	extern HWND g_hWnd;
	extern HWND g_hMainDlg;
	extern HANDLE g_hComm;
	extern COMMCONFIG g_cc;

	extern BOOL g_hRunReadThread;
	extern int g_ENQsSent;

	/*Thread handles*/
	extern HANDLE g_hReadThread;
	extern HANDLE g_hWriteThread;
	extern HANDLE g_hIdleWaitThread;
	extern HANDLE g_hIdleSendENQThread;
	extern HANDLE g_hWaitConnectThread;
	extern HANDLE g_hWaitForACKThread;

	extern HANDLE g_hReceivingThread;
	extern HANDLE g_hWaitForSYNThread;
	extern HANDLE g_hReadForSYNThread;

	extern HANDLE g_hTerminateThreadEvent;
	extern HANDLE g_hEnqEvent;
	extern HANDLE g_hAckEvent;
	extern HANDLE g_hRXSynEvent;
}