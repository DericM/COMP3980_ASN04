/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: rx_check_error.cpp : resposible for checking errors in the recieved packet using the CRC
--
-- PROGRAM: Animal Farm
--
-- FUNCTIONS:
-- bool rxce_check_error(std::string data, uint16_t syndrome)
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "packetDefine.h"
#include "send.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: rxce_check_error(std::string data, uint16_t syndrome)
--
-- NOTES:
-- STakes in a srting and syndrome then checks if the data has an error usinfg the CRC
----------------------------------------------------------------------------------------------------------------------*/
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