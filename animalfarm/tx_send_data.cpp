/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: tx_send_data.cpp : is Responsible for taking in a packet that is to be sent and send that packet through the port.
--
-- PROGRAM: Animal Farm
--
-- FUNCTIONS:
-- bool txsd_setup(const char* frame)
-- bool txsd_send(const char* frame)
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include <stdexcept>
#include "GlobalVar.h"
#include "send.h"
#include "tx_wait_ack.h"
#include "tx_send_data.h"


#define TX_FAIL_COUNTER 3
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: xsd_setup(const char* frame)
--
-- NOTES:
-- Takes in a packet and passes it to the txsd_sed() function to send and keeps track of how many times the program 
-- attempted to send the packet it the program has more than 3 attempts it exits back to Idle
----------------------------------------------------------------------------------------------------------------------*/
bool txsd_setup(const char* frame) {
	int transmission_attempts = 0;
	while (transmission_attempts < TX_FAIL_COUNTER)
	{
		if (txsd_send(frame))
		{
			return true;//go get next packet
		}
		else
		{
			transmission_attempts++;
			PurgeComm(GlobalVar::g_hComm, PURGE_TXCLEAR);
		}
	}
	return false;//return to idle
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: txsd_send(const char* frame)
--
-- NOTES:
-- attempts to send the frame over the comm port by calling ipc_send_packet() from the receive.cpp
----------------------------------------------------------------------------------------------------------------------*/
bool txsd_send(const char* frame) {
	//send packet
	if (!ipc_send_packet(frame)) {
		return false;//resend packet
	}
	//goto wait for ack.
	if (!txwa_receive_ack()) {
		return false;//resend packet
	}
	return true;
}

