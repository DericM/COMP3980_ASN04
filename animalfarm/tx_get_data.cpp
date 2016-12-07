#include "stdafx.h"
#include "tx_get_data.h"
#include "animalfarm.h"
#include "idle.h"
#include <string>
#include <fstream>
#include "GlobalVar.h"
#include "tx_wait_connect.h"
#include "send.h"
#include "packetDefine.h"
#include "tx_send_data.h"

using namespace std;


char packetize [HEADER_SIZE + DATA_SIZE + CRC_SIZE];

static size_t packetCounter = 0;
char packet_flag;


bool txgd_setup() {
	//gets file for directory path
	ifstream file;
	file.open(ExePathA() + "\\test.txt", std::ios::binary);

	packetCounter = 0;
	packet_flag = 's';

	//checks if file is open
	if (!file.is_open())
	{
		MessageBoxW(GlobalVar::g_hWnd, L"Cannot open file!", 0, 0);
		return false;
	}
	GlobalVar::g_vFileBuffer.clear();
	//passes characters from vector to buffer
	GlobalVar::g_vFileBuffer = vector<char>((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

	file.close();

	//chekcs wif buffer is empty
	if (GlobalVar::g_vFileBuffer.size() == 0)
	{
		MessageBoxW(GlobalVar::g_hWnd, L"The file is empty!", 0, 0);
		return false;
	}

	return true;
}




bool txgd_get_next_packet() {
	if (packetCounter == 0)
		txgd_setup();

	char syn = 0x16;

	char packetBuffer[DATA_SIZE + 1];
	size_t curPos = packetCounter * DATA_SIZE;
	size_t curDataSize = GlobalVar::g_vFileBuffer.size() - curPos < DATA_SIZE ? GlobalVar::g_vFileBuffer.size() - curPos : DATA_SIZE;
	if (curDataSize < DATA_SIZE)
	{
		memset(packetBuffer, 'x', DATA_SIZE);
		packetCounter = 0;
		GlobalVar::g_bWannaSendFile = false;
	}
	if (curPos < GlobalVar::g_vFileBuffer.size())
		memcpy_s(packetBuffer, DATA_SIZE, &GlobalVar::g_vFileBuffer[curPos], curDataSize);

	packetBuffer[DATA_SIZE] = '\0';
	std::string strData(packetBuffer);
	uint16_t crc = calculateCRC16(strData);

	memcpy_s(packetize, HEADER_SIZE, &syn, HEADER_SIZE);
	memcpy_s(packetize + HEADER_SIZE, DATA_SIZE, packetBuffer, DATA_SIZE);
	memcpy_s(packetize + HEADER_SIZE + DATA_SIZE, CRC_SIZE, &crc, CRC_SIZE);

	if (txsd_setup(packetize)) {
		packetCounter++;
		return true;
	}
	else
	{
		packetCounter = 0;
		GlobalVar::g_bWannaSendFile = false;
	}

	return false;
}


