#include "stdafx.h"
#include "packetDefine.h"
#include "send.h"



bool rxce_check_error(char syn, std::string data, uint16_t syndrome) {

	if (syn != 0x16) {
		LOGMESSAGE(L"Sync bit missmatch\n");
		return false;
	}

	uint16_t derived_syndrome = calculateCRC16(data);
	if (syndrome != derived_syndrome) {
		LOGMESSAGE(L"Error in packet\n");
		return false;
	}

	//call toprocess data////
	LOGMESSAGE(L"packet: " << data.c_str() << "/n");
	////////////////////

	ipc_send_ack();
	return true;
}