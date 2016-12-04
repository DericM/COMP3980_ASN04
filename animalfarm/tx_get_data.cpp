#include "stdafx.h"
#include "tx_get_data.h"
#include "animalfarm.h"
#include "idle.h"
#include <string>
#include <fstream>
#include "GlobalVar.h"
#include "tx_wait_connect.h"
#include "send.h"
#include <vector>


using namespace std;

char buff[1024];
char buffer[];
DWORD dwBytesRead;
HANDLE hdlF;
int counter;
string packets;
char syn = 0x16;
char packetize [1027];

/*void getData() {
	openFile(TEXT("\\TEST.TXT"));
}*/

string data;
vector <string> v;


DWORD WINAPI openFile(const HWND *box, LPCWSTR pFile) {

	LOGMESSAGE(L"IN GET DATA \n");
	//idle_go_to_idle();		???

	LOGMESSAGE(L"starting to open file\n");
	int sendLines;
	int idx;
	string tmp;
	string myfile;
	myfile = "C:\\Users\\Yiaoping\\Desktop\\test3.txt";
	/*ifstream file(myfile);

	
	SendMessageA(*box, EM_SETREADONLY, (LPARAM)FALSE, NULL);

	if (file.is_open()) {
		LOGMESSAGE(L"FILE IS OPEN\n");
		while (getline(file, tmp)) {
			tmp += '\r\n';
			idx = GetWindowTextLength(*box);
			SendMessageA(*box, EM_SETSEL, (LPARAM)idx, (LPARAM)idx);
			SendMessageA(*box, EM_REPLACESEL, 0, (LPARAM)(tmp.c_str()));
		}
		sendLines = SendMessageA(*box, EM_GETLINECOUNT, NULL, NULL);
	}else
		LOGMESSAGE(L"FILE NOT ABLE TO OPEN");

	file.read(buff, 1024);*/

	readFile(buff);


	return 0;
}

//returns true if it can find a special character
bool hasSpecialChars(LPCWSTR pFile) {
	//if (pFile(""))		
		//return true;
	//else 
		return false;
}

//if last packet was successfully sent (LastPacketACK Bool == true && FinalPacketSent == False)
//
int readFile(char* buff) {

	packets = makePacket(buff);
	strcpy_s(packetize, packets.c_str());


	if (!ipc_send_packet(packetize)) {
		//break;
		return 0;
	}
	
	return 0;
}

string makePacket(char buff[])
{	

	std::string rtn;
	rtn.push_back(syn);
	rtn.append(buff);
	uint16_t crc = calculateCRC16(buff);
	rtn.push_back(crc >> 8);
	rtn.push_back(0x00FF & crc);
	//append null characters IF EOF;
	return rtn;

}

//check for EOF




/*char* createPacket(char data[]) {


	uint16_t crc = gen_crc16(packet, sizeof(packet));
	char temp = (char)SYN;
	memcpy_s(packet, 1, &temp, sizeof(temp));
	memcpy_s(packet + 1, 1024, data, 1024);
	memcpy_s(packet + 1024, 2, &crc, sizeof(uint16_t));
	return packet;

}*/



uint16_t calculateCRC16(const std::string& data) {
	static constexpr auto poly = 0x8005;
	auto size = data.size();
	uint16_t out = 0;
	int bits_read = 0;
	bool bit_flag;

	std::vector<char> bytes(data.begin(), data.end());

	int i = 0;
	while (size > 0) {
		bit_flag = (out >> 15) != 0;

		/* Get next bit: */
		// item a) work from the least significant bits
		out = (out << 1) | ((bytes[i] >> bits_read) & 1);

		/* Increment bit counter: */
		if (++bits_read > 7) {
			bits_read = 0;
			i++;
			size--;
		}

		/* Cycle check: */
		if (bit_flag) {
			out ^= poly;
		}
	}

	// item b) "push out" the last 16 bits
	for (int i = 0; i < 16; ++i) {
		out = (out << 1) ^ (poly * ((out >> 15) != 0));
	}

	// item c) reverse the bits
	uint16_t crc = 0;
	for (int i = 0x8000, j = 0x001; i; i >>= 1, j <<= 1) {
		if (i & out) {
			crc |= j;
		}
	}
	return crc;
}


