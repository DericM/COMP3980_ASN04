#include "stdafx.h"
#include "globalvar.h"
#include "packetDefine.h"
#include "rx_parse_packet.h"

#include <vector>

std::vector<char> buffer;
char checkSyn = 0x16;

void rx_pp_parse(const char* packet) {
	char dataBuffer[DATA_SIZE + 1];
	uint16_t crc;
	memcpy_s(dataBuffer, DATA_SIZE, packet + HEADER_SIZE, DATA_SIZE);
	memcpy_s(&crc, CRC_SIZE, packet+  HEADER_SIZE + DATA_SIZE, CRC_SIZE);
	dataBuffer[DATA_SIZE] = '\0';
	std::string strData(dataBuffer);

	uint16_t checkCrc = calculateCRC16(strData);
	if (crc != checkCrc)
		LOGMESSAGE(L"Decode error! crc is different!" << std::endl);

	int idx = GetWindowTextLength(GlobalVar::g_hSendBox);
	SendMessageA(GlobalVar::g_hSendBox, EM_SETSEL, (LPARAM)idx, (LPARAM)idx);
	SendMessageA(GlobalVar::g_hSendBox, EM_REPLACESEL, 0, (LPARAM)strData.c_str());
}