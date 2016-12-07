#include "stdafx.h"
#include "tx_wait_ack.h"
#include "tx_get_data.h"
#include "receive.h"
#include "idle.h"
#include "globalvar.h"
#include "packetDefine.h"

/*
	Waits for ACK after sending a packet.
*/
bool txwa_receive_ack() {
	DWORD packetSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	DWORD ACK_TIMER = static_cast<DWORD>(ceil(8.0 * packetSize / GlobalVar::g_cc.dcb.BaudRate * 1000) * 3);
	ACK_TIMER = 3000;
	if (ipc_recieve_ack(ACK_TIMER)) {
		return true;//success
	}

	return false;//failed to send packet
}
