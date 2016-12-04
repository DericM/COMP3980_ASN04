#include "stdafx.h"
#include <stdexcept>
#include "globalvar.h"


/*
* Open the comm port.
*/
bool is_open_port( LPCWSTR& lpszCommName) {
	//set comm settings
	GlobalVar::g_cc.dwSize = sizeof(COMMCONFIG);
	GlobalVar::g_cc.wVersion = 0x100;
	GetCommConfig(GlobalVar::g_hComm, &GlobalVar::g_cc, &GlobalVar::g_cc.dwSize);
	if (!CommConfigDialog(lpszCommName, GlobalVar::g_hWnd, &GlobalVar::g_cc)) {
		MessageBoxW(GlobalVar::g_hWnd, L"Failed to configure Comm Settings", 0, 0);
		return false;
	}

	//open comm port
	GlobalVar::g_hComm = CreateFile(
		lpszCommName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL
	);

	//check for failure
	if (GlobalVar::g_hComm == INVALID_HANDLE_VALUE) {
		MessageBoxW(GlobalVar::g_hWnd, L"Failed to open Comm Port", 0, 0);
		return false;
	}

	SetCommState(GlobalVar::g_hComm, &GlobalVar::g_cc.dcb);

	return true;
}



/*
* close port.
*/
void is_close_port() {
	//CloseHandle(osReader.hEvent);
	CloseHandle(GlobalVar::g_hComm);
}


