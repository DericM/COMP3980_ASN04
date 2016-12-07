/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: rx_parse_packet.cpp: Parses the packet and prepares it for Error Check
--
-- PROGRAM: Animal Farm
--
-- FUNCTIONS:
-- bool rxpp_parse_packet(const char* packet)
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "globalvar.h"
#include "packetDefine.h"
#include "rx_parse_packet.h"
#include "rx_check_error.h"
#include <fstream>


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: rrxpp_parse_packet(const char* packet)
--
-- NOTES:
-- takes in a packet and separeates the data from the CRC and passes both into Error Check 
----------------------------------------------------------------------------------------------------------------------*/
bool rxpp_parse_packet(const char* packet) {
	char dataBuffer[DATA_SIZE + 1];
	uint16_t syndrome;
	memcpy_s(dataBuffer, DATA_SIZE, packet, DATA_SIZE);
	memcpy_s(&syndrome, CRC_SIZE, packet + DATA_SIZE, CRC_SIZE);
	dataBuffer[DATA_SIZE] = '\0';
	std::string strData(dataBuffer);
	std::string tempStr = "";
	size_t lastChar = strData.find_first_of('x');
	if (lastChar != std::string::npos)
		tempStr = strData.substr(0, lastChar);

	if (dataBuffer[0] == 0x11)
	{
		GlobalVar::g_vInputBuffer.clear();
		tempStr = "Start Packet\r\n";
	}
	else if (dataBuffer[0] == 0x14)
	{
		std::ofstream of(ExePathA() + "\\output.txt", std::ios::binary);
		for (size_t i = 0; i < GlobalVar::g_vInputBuffer.size(); i++)
			of << GlobalVar::g_vInputBuffer[i];

		of.close();
	}
	else
	{
		GlobalVar::g_vInputBuffer.insert(GlobalVar::g_vInputBuffer.end(), dataBuffer, dataBuffer+DATA_SIZE);
	}
	
	std::ostringstream stringStream;
	stringStream << strData.length() << " : " << tempStr << std::endl;
	tempStr = stringStream.str();

	SendMessageA(GlobalVar::g_hSendBox, EM_REPLACESEL, 0, (LPARAM)tempStr.c_str());

	if (rxce_check_error(strData, syndrome)) {
		return true;
	}

	return false;
}