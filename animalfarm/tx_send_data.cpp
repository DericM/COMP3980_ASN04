#include "stdafx.h"
#include <stdexcept>
#include "GlobalVar.h"
#include "send.h"



/*
SP Set Up
	Take in Frame
	Go to SP Write
	If Write packet to port fails
		go back to IDLE Set Up
	go to Wait For Ack WFA Wait
*/
void txsd_send(char frame[1027]) {
	LOGMESSAGE(L"\nEntering: txsd_send() - ");

	//send packet
	if (!ipc_send_packet(frame)) {
		return;//return to idle.
	}

	//goto wait for ack.
	
}

