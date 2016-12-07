#include "stdafx.h"
#include "globalvar.h"

namespace GlobalVar
{
	HWND g_hWnd;
	HWND g_hMainDlg;
	HWND g_hSendBox;
	HANDLE g_hComm;
	COMMCONFIG g_cc;

	int g_sending_file;


	DWORD TXWC_ACK_TIMER = 100;//tx_wait_connect
	//TXWC_ACK_TIMER = static_cast<DWORD>(ceil(16.0 / GlobalVar::g_cc.dcb.BaudRate * 1000));

	DWORD TXWA_ACK_TIMER = 3000;

	DWORD IDLE_SEQ_TIMEOUT = 1000;

	DWORD RXWP_SYN_TIMER = 3000;
	//SYN_TIMER = static_cast<DWORD>(ceil(8.0 * packetSize / GlobalVar::g_cc.dcb.BaudRate * 1000) * 3);
	
	bool g_bRunIdle = false;
	HANDLE g_hIdleThread;
	HANDLE g_hTerminateIdleEvent;

}