#include "stdafx.h"
#include "globalvar.h"
#include "packetDefine.h"
#include "rx_parse_packet.h"
#include "rx_check_error.h"

bool rxpp_parse_packet(const char* packet) {
	char dataBuffer[DATA_SIZE + 1];
	uint16_t syndrome;
	memcpy_s(dataBuffer, DATA_SIZE, packet, DATA_SIZE);
	memcpy_s(&syndrome, CRC_SIZE, packet+  HEADER_SIZE + DATA_SIZE, CRC_SIZE);
	dataBuffer[DATA_SIZE] = '\0';
	std::string strData(dataBuffer);
	size_t lastChar = strData.find_first_of('x');
	if (lastChar != std::string::npos)
		strData = strData.substr(0, lastChar);

	SendMessageA(GlobalVar::g_hSendBox, EM_REPLACESEL, 0, (LPARAM)strData.c_str());

	if (rxce_check_error(strData, syndrome)) {
		return true;
	}
	return false;
}