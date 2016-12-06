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
}