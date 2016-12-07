#include "stdafx.h"
#include "tx_wait_ack.h"
#include "tx_get_data.h"
#include "receive.h"
#include "idle.h"
#include "globalvar.h"

/*
	Waits for ACK after sending a packet.
*/
bool txwa_receive_ack() {
	if (ipc_recieve_ack(GlobalVar::TXWA_ACK_TIMER)) {
		return true;//success
	}
	return false;//failed to send packet
}
