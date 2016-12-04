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

char syn = 0x16;
char packetize [HEADER_SIZE + DATA_SIZE + CRC_SIZE];


DWORD WINAPI openFile(const HWND *box, LPCWSTR pFile) {

	char packet_flag = 's';

	ifstream file(ExePathA() + "\\test3.txt", std::ios::binary);
	if (!file.is_open())
	{
		MessageBoxW(GlobalVar::g_hWnd, L"Cannot open file!", 0, 0);
		return 0;
	}

	std::vector<char> buffer((
		std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()));

	if (buffer.size() == 0)
	{
		MessageBoxW(GlobalVar::g_hWnd, L"The file is empty!", 0, 0);
		return 0;
	}

	static size_t packetCounter = 0;

	while (packet_flag != 'x') {
		size_t curFilePos = packetCounter * DATA_SIZE;
		char packetBuffer[DATA_SIZE + 1];

		size_t remain = buffer.size() - curFilePos - 1;


		switch (packet_flag) {
		case 's':
			memset(packetBuffer, 's', sizeof(packetBuffer));
			packet_flag = 'm';
			break;
		case 'm':
			if (remain < DATA_SIZE) {
				memset(packetBuffer, '\0', sizeof(packetBuffer));
				memcpy_s(packetBuffer, DATA_SIZE, &buffer[curFilePos], remain);
				packet_flag = 'e';
			}
			else {
				memcpy_s(packetBuffer, DATA_SIZE, &buffer[curFilePos], DATA_SIZE);
			}
			packetCounter++;
			break;
		case 'e':
			memset(packetBuffer, 'x', sizeof(packetBuffer));
			packet_flag = 'x';
			break;
		}

		packetBuffer[DATA_SIZE] = '\0';
		std::string strData(packetBuffer);
		uint16_t crc = calculateCRC16(strData);

		memcpy_s(packetize, HEADER_SIZE, &syn, HEADER_SIZE);
		memcpy_s(packetize + HEADER_SIZE, DATA_SIZE, packetBuffer, DATA_SIZE);
		memcpy_s(packetize + HEADER_SIZE + DATA_SIZE, CRC_SIZE, &crc, CRC_SIZE);

		/*if (!ipc_send_packet(packetize)) {
			break;
		}*/

		if (!txsd_setup(packetize)) {
			break;
		}
	}
	file.close();
	return 0;
}


