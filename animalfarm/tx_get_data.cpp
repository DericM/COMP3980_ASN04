/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: tx_get_data.cpp : contains the functions that are responsible for getting the data from the file to be sent
--                                , packitizing the data and grabing packets to pass into send data.
--
-- PROGRAM: Animal Farm
--
-- FUNCTIONS:
-- bool txgd_setup()
-- bool txgd_get_next_packet()
----------------------------------------------------------------------------------------------------------------------*/

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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: txgd_setup()
--
-- NOTES:
-- If new file is being sent this function gwets call to grab all the data from the file and save it into a vector that holds chars
-- so that they can be accessed more easily 
----------------------------------------------------------------------------------------------------------------------*/
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



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:  txgd_get_next_packet() 
--
-- NOTES:
-- Grabs the next packet that need to be sent from the vector of chars and passes it to txsd_setup in tx_send_data.cpp
----------------------------------------------------------------------------------------------------------------------*/
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
		GlobalVar::g_sending_file = false;
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

	return false;
}


