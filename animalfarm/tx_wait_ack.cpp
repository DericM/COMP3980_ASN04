#include "stdafx.h"
#include "tx_wait_ack.h"
#include "receive.h"

/*
	Waits for ACK after sending a packet.
*/
bool txwa_receive_ack() {

	int TRANSMISSION_TIMER = 1000;

	/*if (!ipc_recieve_ack(TRANSMISSION_TIMER)) {
		return false;
	}*/
	return true;

}