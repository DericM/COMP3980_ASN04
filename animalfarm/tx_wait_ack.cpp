#include "stdafx.h"
#include "tx_wait_ack.h"
#include "receive.h"
#include "globalvar.h"
#include "packetDefine.h"
#include "idle.h"
#include "tx_send_data.h"

double packetSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
int TRANSMISSION_TIMER = ceil(packetSize / GlobalVar::g_cc.dcb.BaudRate * 1000);
/*
	Waits for ACK after sending a packet.
*/
DWORD WINAPI txwa_receive_ack() {
	

	if (!ipc_recieve_ack(TRANSMISSION_TIMER, &GlobalVar::g_hWaitForACKThread, txwa_receive_ack_event)) {
		//Timeout
		//return false;
	} else {
		//success

	}
	return 0;
}

DWORD WINAPI txwa_receive_ack_event(LPVOID pData_)
{
	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hAckEvent, TRANSMISSION_TIMER);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		//take 1024 from the buffer
		// Received ack;
		GlobalVar::g_TransmissionAttempts = 0;
		idle_go_to_idle();
		break;

	case WAIT_TIMEOUT:
		// Not receieved ack.
		txsd_send();
		break;

	default:
		break;
	}

	ResetEvent(GlobalVar::g_hAckEvent);

	return 0;
}