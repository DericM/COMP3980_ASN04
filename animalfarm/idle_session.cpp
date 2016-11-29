#include "stdafx.h"
#include <stdexcept>
#include "globalvar.h"


/*
* Open the comm port.
*/
void is_open_port(HWND& hWnd, LPCWSTR& lpszCommName) {
	LOGMESSAGE(L"Entering: idle_open_port()\n");

	//set comm settings
	GlobalVar::g_cc.dwSize = sizeof(COMMCONFIG);
	GlobalVar::g_cc.wVersion = 0x100;
	GetCommConfig(GlobalVar::g_hComm, &GlobalVar::g_cc, &GlobalVar::g_cc.dwSize);
	if (!CommConfigDialog(lpszCommName, hWnd, &GlobalVar::g_cc)) {
		throw std::runtime_error("Failed to configure Comm Settings");
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
		throw std::runtime_error("Failed to open Comm Port");
	}

	SetCommState(GlobalVar::g_hComm, &GlobalVar::g_cc.dcb);
}



/*
* close port.
*/
void is_close_port() {
	LOGMESSAGE(L"Entering: idle_close_port()\n");

	//CloseHandle(osReader.hEvent);
	CloseHandle(GlobalVar::g_hComm);
}


