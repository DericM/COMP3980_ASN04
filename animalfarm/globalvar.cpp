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

	DWORD T_IDLE = 1000;
	DWORD T_ACK  = 75;
	DWORD T_TX   = 1000;
	DWORD T_RX   = 3 * T_TX;
	DWORD T_WAIT = 3 * T_RX;

}