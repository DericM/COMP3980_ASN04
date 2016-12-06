#include "stdafx.h"
#include "tx_wait_ack.h"
#include "tx_get_data.h"
#include "receive.h"
#include "idle.h"

int TRANSMISSION_TIMER = 1000;



/*
	Waits for ACK after sending a packet.
*/
bool txwa_receive_ack() {
	if (ipc_recieve_ack(TRANSMISSION_TIMER)) {
		return true;//success
	}
	return false;//failed to send packet
}
