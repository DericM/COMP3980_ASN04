#include "stdafx.h"
#include "packetDefine.h"
#include "send.h"



bool rxce_check_error(std::string data, uint16_t syndrome) {

	uint16_t derived_syndrome = calculateCRC16(data);

	if (syndrome != derived_syndrome) {
		LOGMESSAGE(L"error in packet");
		return false;
	}

	//call toprocess data////
	LOGMESSAGE(L"packet" << data.c_str());
	////////////////////

	ipc_send_ack();
	return true;
}