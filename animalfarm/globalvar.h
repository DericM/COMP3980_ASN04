#pragma once

namespace GlobalVar
{
	extern HWND g_hWnd;
	extern HWND g_hMainDlg;
	extern HWND g_hSendBox;	//dialog box for writing file to
	extern HANDLE g_hComm;
	extern COMMCONFIG g_cc;



	extern int g_sending_file;


	extern DWORD T_IDLE;
	extern DWORD T_ACK;
	extern DWORD T_TX;
	extern DWORD T_RX;


}