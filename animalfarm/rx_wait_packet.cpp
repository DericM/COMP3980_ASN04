#include "stdafx.h"
#include "globalvar.h"
#include "tx_wait_connect.h"
#include "idle.h"
#include "rx_wait_packet.h"
#include "receive.h"
#include "rx_parse_packet.h"
#include "packetDefine.h"

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
char pack[HEADER_SIZE + DATA_SIZE + CRC_SIZE];

BOOL rxwp_setUp() {
	double packetSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	SYN_TIMER = ceil(packetSize / GlobalVar::g_cc.dcb.BaudRate * 1000) * 3;

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
	GlobalVar::g_hReadForSYNThread = CreateThread(NULL, 0, rx_read_for_syn, NULL, 0, 0);

	return TRUE;
}

DWORD WINAPI rx_read_for_syn(LPVOID pData_)
{
	if (!ipc_recieve_packet(pack, synParam.timer, &GlobalVar::g_hReadForPACKThread, rx_wait_pack)) {
		//timeout
	}
	else {
		//success
	}

	//if (!ipc_recieve_syn(SYN_TIMER, &GlobalVar::g_hWaitForSYNThread, rx_wait_syn)) {
	//	//timeout
	//}
	//else {
	//	//success
	//}

	return 0;
}

void HandleReceivedSYN()
{
	SetEvent(GlobalVar::g_hReadForSYNThread);
}

DWORD WINAPI rx_wait_syn(LPVOID pData_)
{
	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hRXSynEvent, SYN_TIMER);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		// Received SYN;
		if (!ipc_recieve_packet(pack, synParam.timer, &GlobalVar::g_hReadForPACKThread, rx_wait_pack)) {
			//timeout
		} else {
			//success
		}
		break;

	case WAIT_TIMEOUT:
		// Not receieved SYN.
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


DWORD WINAPI rx_wait_pack(LPVOID pData_) {
	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hRXPackEvent, synParam.timer);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		// Received Packet;
		rx_pp_parse(pack);
		break;

	case WAIT_TIMEOUT:
		// Not receieved Packet.
		ipc_terminate_read_thread(GlobalVar::g_hReadForPACKThread);
		idle_go_to_idle();
		break;

	default:
		idle_go_to_idle();
		break;
	}

	ResetEvent(GlobalVar::g_hReadForPACKThread);

	return 0;
}


