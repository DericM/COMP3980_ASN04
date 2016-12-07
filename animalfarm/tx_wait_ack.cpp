/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: tx_wait_ack.cpp : is Responsible for wait for and ack after the program has sent a packet.   
--
-- PROGRAM: Animal Farm
--
-- FUNCTIONS:
-- bool txwa_receive_ack()
----------------------------------------------------------------------------------------------------------------------*/

#include "stdafx.h"
#include "tx_wait_ack.h"
#include "tx_get_data.h"
#include "receive.h"
#include "idle.h"
#include "globalvar.h"
#include "packetDefine.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: txwa_receive_ack()
--
-- NOTES:
-- Creates and event to wait for and ACK character to be read from the comm port and pairs it with the timer. If timer times out 
-- function returns and program tries to send again. If Ack is recieved function returnus program to Idle
----------------------------------------------------------------------------------------------------------------------*/
bool txwa_receive_ack() {
	DWORD packetSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	DWORD ACK_TIMER = static_cast<DWORD>(ceil(8.0 * packetSize / GlobalVar::g_cc.dcb.BaudRate * 1000) * 3);
	ACK_TIMER = 3000;
	if (ipc_recieve_ack(ACK_TIMER)) {
		return true;//success
	}

	return false;//failed to send packet
}
