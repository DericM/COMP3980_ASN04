#include "stdafx.h"
#include "tx_wait_connect.h"
#include "tx_get_data.h"
#include "receive.h"
#include "globalvar.h"


//Waiting for An ACK
bool txwc_wait_connect_ack() {
	//Wait for ACK Connect timeout
	DWORD ACK_TIMER = static_cast<DWORD>(ceil(16.0 / GlobalVar::g_cc.dcb.BaudRate * 1000));
	ACK_TIMER = 500;
	//Tries to recieve ack returns true is ack is recieved returns false if timed out
	if (ipc_recieve_ack(ACK_TIMER))
	{
		if (txgd_get_next_packet())
			return true;
		else
			LOGMESSAGE(L"Sending a packet failed." << std::endl);
	}

	PurgeComm(GlobalVar::g_hComm, PURGE_TXCLEAR);

	return false;//failed to send packet
}




