#include "stdafx.h"
#include "globalvar.h"
#include "tx_wait_connect.h"
#include "idle.h"
#include "tx_get_data.h"
#include "receive.h"

#include <stdexcept>
#include <memory>
#include <math.h>

/*struct ConnectParams
{
	int enqCounter;
	int timer;
	OVERLAPPED reader;
};
ConnectParams conParam;
*/

struct AckParams
{
	int timer;
	std::wstring filename;
};
AckParams ackParam;

int ACK_TIMER;


BOOL txwc_setup(const std::wstring& fileName) {
	GlobalVar::g_ENQsSent++;
	ACK_TIMER = 10;//(ceil(16.0 / GlobalVar::g_cc.dcb.BaudRate * 1000));


	GlobalVar::g_hAckEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL    // object name
	);

	ackParam.timer = ACK_TIMER;
	ackParam.filename = fileName;

	//TerminateThread(GlobalVar::g_hWaitConnectThread, 0);
	//TerminateThread(GlobalVar::g_hWaitForACKThread, 0);
	//CloseHandle(GlobalVar::g_hWaitConnectThread);
	//CloseHandle(GlobalVar::g_hWaitForACKThread);
	GlobalVar::g_hWaitConnectThread = CreateThread(NULL, 0, txwc_receive_ack, NULL, 0, 0);

	return TRUE;
}

DWORD WINAPI txwc_receive_ack(LPVOID pData_)
{
	if (!ipc_recieve_ack(ACK_TIMER, &GlobalVar::g_hWaitForACKThread, txwc_receive_ack_event)) {
		//timeout
	}
	else {
		//success?
	}
	
	return 0;
}


DWORD WINAPI txwc_receive_ack_event(LPVOID pData_)
{
	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hAckEvent, ackParam.timer);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		/*if (ackParam.filename.length() == 0)
			idle_go_to_idle();
		else*/
			
			GlobalVar::g_ENQsSent = 0;
			openFile(&GlobalVar::g_hSendBox, L"Test.txt");
			//take 1024 from the buffer
		// Received ack;
		break;

	case WAIT_TIMEOUT:
		// Not receieved ack.
		ipc_terminate_read_thread(GlobalVar::g_hWaitConnectThread);
		idle_go_to_idle();
		break;

	default:
		break;
	}

	ResetEvent(GlobalVar::g_hAckEvent);

		return 0;
}

