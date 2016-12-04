#include "stdafx.h"
#include "globalvar.h"
#include "tx_wait_connect.h"
#include "idle.h"
#include "rx_wait_packet.h"
#include "receive.h"
#include "rx_parse_packet.h"

#include <stdexcept>
#include <memory>
#include <math.h>

struct SynParams
{
	HWND hWnd;
	int timer;
};
SynParams synParam;

int SYN_TIMER;

BOOL rxwp_setUp() {

	LOGMESSAGE(L"\nEntering: rxwp_setUP\n");
	SYN_TIMER = (ceil(8216 / GlobalVar::g_cc.dcb.BaudRate * 1000) * 3);

	GlobalVar::g_hRXSynEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL    // object name
		);


	synParam.timer = SYN_TIMER;

	//TerminateThread(GlobalVar::g_hWaitForSYNThread, 0);
	//TerminateThread(GlobalVar::g_hReadForSYNThread, 0);
	//CloseHandle(GlobalVar::g_hWaitForSYNThread);
	//CloseHandle(GlobalVar::g_hReadForSYNThread);
	GlobalVar::g_hWaitForSYNThread = CreateThread(NULL, 0, rx_wait_syn, NULL, 0, 0);
	GlobalVar::g_hReadForSYNThread = CreateThread(NULL, 0, rx_read_for_syn, NULL, 0, 0);

	return TRUE;
}

DWORD WINAPI rx_read_for_syn(LPVOID pData_)
{

	if (!ipc_recieve_syn(SYN_TIMER)) {
		//timeout
	}
	else {
		//success
	}

	return 0;
}

void HandleReceivedSYN()
{
	SetEvent(GlobalVar::g_hReadForSYNThread);
}

DWORD WINAPI rx_wait_syn(LPVOID pData_)
{
	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hRXSynEvent, synParam.timer);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		// Received SYN;
		char pack[1026];
		ipc_recieve_packet(&pack[0]);
		rx_pp_parse(pack);
		break;

	case WAIT_TIMEOUT:
		// Not receieved SYN.
		LOGMESSAGE(L"TIMED OUT IN WAIT FOR PACKET" << std::endl);
		ipc_terminate_read_thread(GlobalVar::g_hReadForSYNThread);
		idle_go_to_idle();
		break;

	default:
		idle_go_to_idle();
		break;
	}

	ResetEvent(GlobalVar::g_hReadForSYNThread);

	return 0;
}


