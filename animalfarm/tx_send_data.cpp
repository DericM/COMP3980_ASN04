#include "stdafx.h"
#include <stdexcept>
#include "GlobalVar.h"
#include "send.h"
#include "tx_wait_ack.h"
#include "tx_send_data.h"


#define TX_FAIL_COUNTER 3

bool txsd_setup(const char* frame) {
	int transmission_attempts = 0;
	while (transmission_attempts < TX_FAIL_COUNTER) {
		if (txsd_send(frame)) {
			return true;//go get next packet
		}
		transmission_attempts++;

		PurgeComm(GlobalVar::g_hComm, PURGE_TXABORT);
		//PurgeComm(GlobalVar::g_hComm, PURGE_TXCLEAR);
	}
	return false;//return to idle
}


bool txsd_send(const char* frame) {
	//send packet
	if (!ipc_send_packet(frame)) {
		return false;//resend packet
	}
	//goto wait for ack.
	//if (!txwa_receive_ack()) {
	//	return false;//resend packet
	//}
	return true;
}

