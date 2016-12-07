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

	DWORD TXWA_ACK_TIMER = 1000;

	DWORD IDLE_SEQ_TIMEOUT = 500;

	DWORD RXWP_SYN_TIMER = 2000;
	//SYN_TIMER = static_cast<DWORD>(ceil(8.0 * packetSize / GlobalVar::g_cc.dcb.BaudRate * 1000) * 3);

}