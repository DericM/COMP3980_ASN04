#include "stdafx.h"
#include <stdexcept>
#include "GlobalVar.h"
#include "send.h"
#include "tx_wait_ack.h"
#include "tx_send_data.h"
#include "packetDefine.h"
#include "idle.h"


#define TX_FAIL_COUNTER 3
const char* packet;

bool txsd_setup(const char* frame) {
	packet = frame;
	GlobalVar::g_TransmissionAttempts = 0;
	txsd_send();

	return false;//return to idle
}


void txsd_send() {
	//send packet
	if (GlobalVar::g_TransmissionAttempts < TX_FAIL_COUNTER) {
		GlobalVar::g_TransmissionAttempts++;
		if (!ipc_send_packet(packet)) {
			//return false;//resend packet
		}
		//goto wait for ack.
		if (!txwa_receive_ack()) {
			//return false;//resend packet
		}
	}
	else {
		idle_go_to_idle();
	}
	
}

