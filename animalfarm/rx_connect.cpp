#include "stdafx.h"
#include "send.h"
#include "rx_wait_packet.h"


bool rxc_send_ack() {

	ipc_send_ack();

	if (rxwp_wait_for_packet()) {
		return true;
	}
	return false;
}




	