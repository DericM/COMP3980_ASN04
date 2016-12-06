#include "stdafx.h"
#include "tx_wait_connect.h"
#include "tx_get_data.h"
#include "receive.h"
#include "globalvar.h"


//Waiting for An ACK
bool txwc_wait_connect_ack() {
	//Wait for ACK Connect timeout
	DWORD ACK_TIMER = static_cast<DWORD>(ceil(16.0 / GlobalVar::g_cc.dcb.BaudRate * 1000));

	//Tries to recieve ack returns true is ack is recieved returns false if timed out
	if (ipc_recieve_ack(ACK_TIMER)) {
		if (!GlobalVar::g_sending_file) {//check if we are starting a new transfer
			GlobalVar::g_sending_file = true;
			txgd_setup();
		}
		if (txgd_get_next_packet()) {//try to send packet
			return true; //succesfuly sent packet
		}
		GlobalVar::g_sending_file = false;
	}
	return false;//failed to send packet
}




