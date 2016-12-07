/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: rx_connect.cpp : Sends an ACK through the port
--
-- PROGRAM: Animal Farm
--
-- FUNCTIONS:
-- bool rxc_send_ack()
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "send.h"
#include "rx_wait_packet.h"
#include "globalvar.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: rxc_send_ack() 
--
-- NOTES:
-- Calls the rxc_send_ack function from send.cpp 
----------------------------------------------------------------------------------------------------------------------*/
bool rxc_send_ack() {
	ipc_send_ack();

	if (rxwp_wait_for_packet()) {
		PurgeComm(GlobalVar::g_hComm, PURGE_TXCLEAR);
		ipc_send_ack();
		return true;
	}
	return false;
}




	