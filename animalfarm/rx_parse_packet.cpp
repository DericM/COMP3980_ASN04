#include "stdafx.h"
#include "globalvar.h"
#include "packetDefine.h"
#include "rx_parse_packet.h"

#include <vector>

std::vector<char> buffer;
char checkSyn = 0x16;

void rx_pp_parse(const char* packet) {
	char dataBuffer[DATA_SIZE];
	uint16_t crc;
	memcpy_s(dataBuffer, DATA_SIZE, packet, DATA_SIZE);
	memcpy_s(&crc, CRC_SIZE, packet + DATA_SIZE, CRC_SIZE);

	uint16_t checkCrc = calculateCRC16(dataBuffer);
	if (crc != checkCrc)
		LOGMESSAGE(L"Decode error! crc is different!" << std::endl);

	size_t convertedChars = 0;
	wchar_t wcstring[DATA_SIZE + 1];
	mbstowcs_s(&convertedChars, wcstring, DATA_SIZE, dataBuffer, _TRUNCATE);
	wcstring[DATA_SIZE] = L'\0';
	int idx = GetWindowTextLength(GlobalVar::g_hSendBox);
	SendMessageA(GlobalVar::g_hSendBox, EM_SETSEL, (LPARAM)idx, (LPARAM)idx);
	SendMessageA(GlobalVar::g_hSendBox, EM_REPLACESEL, 0, (LPARAM)wcstring);
}