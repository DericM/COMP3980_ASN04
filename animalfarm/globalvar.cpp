#include "stdafx.h"
#include "globalvar.h"

namespace GlobalVar
{
	HWND g_hWnd;
	HWND g_hMainDlg;
	HWND g_hSendBox;
	HANDLE g_hComm;
	COMMCONFIG g_cc;

	BOOL g_hRunReadThread;
	int g_ENQsSent;
	int g_TransmissionAttempts; //how many times we have tried to send a packet
	BOOL g_SendingFile = false;
	BOOL g_IdleSeq = true;

	/*Thread handles*/
	HANDLE g_hReadThread;
	HANDLE g_hWriteThread;
	HANDLE g_hIdleWaitThread;
	HANDLE g_hIdleSendENQThread;

	//transmitting threads
	HANDLE g_hWaitConnectThread;
	HANDLE g_hWaitForACKThread;
	HANDLE g_hReadForSYNThread;
	HANDLE g_hReadForPACKThread;

	//receiving threads
	HANDLE g_hReceivingThread;
	HANDLE g_hWaitForSYNThread;

	HANDLE g_hTerminateThreadEvent;
	HANDLE g_hEnqEvent;
	HANDLE g_hRXSynEvent;
	HANDLE g_hAckEvent;
	HANDLE g_hRXPackEvent;
}