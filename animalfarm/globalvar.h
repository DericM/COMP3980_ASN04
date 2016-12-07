#pragma once

#include <vector>

namespace GlobalVar
{
	extern HWND g_hWnd;
	extern HWND g_hMainDlg;
	extern HWND g_hSendBox;	//dialog box for writing file to
	extern HANDLE g_hComm;
	extern COMMCONFIG g_cc;



	extern int g_sending_file;


	extern DWORD TXWC_ACK_TIMER;
	extern DWORD TXWA_ACK_TIMER;
	extern DWORD IDLE_SEQ_TIMEOUT;
	extern DWORD RXWP_SYN_TIMER;

	extern bool g_bRunIdle;
	extern HANDLE g_hIdleThread;
	extern HANDLE g_hTerminateIdleEvent;
	extern std::vector<char> g_vFileBuffer;
}