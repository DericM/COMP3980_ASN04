#include "stdafx.h"
#include "animalfarm.h"


using namespace std;

char buff[128];
DWORD dwBytesRead;
HANDLE hdlF;
int counter;

int SYN = 0x016;


//opens and connects to file with file name "pFile"
//returns handle to the file to be read
DWORD WINAPI openFile(LPCWSTR pFile, OVERLAPPED osReader) {
	try {
		hdlF = CreateFile(pFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	catch (exception e) {
		cerr << "Unable to create a file";
	}
	
	
	if (hasSpecialChars(pFile) || pFile == NULL) {
		throw "file name is null or has special chars";
		return 0;
	}
	else
		readFile(osReader);
		
}

//returns true if it can find a special character
bool hasSpecialChars(LPCWSTR pFile) {
	//if (pFile(""))		
		return true;
	//else 
		//return false;
}

//if last packet was successfully sent (LastPacketACK Bool == true && FinalPacketSent == False)
//
int readFile(OVERLAPPED osReader) {


	//reads and checks for end of file
	if (!ReadFile(hdlF, buff, 1028, &dwBytesRead, &osReader))
		return 0;				//end of file
		//set data to all null?


	if (LastPacketAck && FinalPacketSent) {
		counter++;
		//increment  counter
	}

	getFilePosition();
						

}

//gets current position of pointer in file
void getFilePosition()
{
	int data[1024];

	int startPacket = 1024 * counter;
	int endPacket = 1024 * (counter+1);

	data[endPacket - startPacket];

	createPacket(*data);


}



void createPacket(int data) {

	char packet[1027];
	
	packet[data + SYN];


	//add crc to packet

}

//pass in dataword and size of data word to create crc codeword
uint16_t gen_crc16(const uint8_t *data, uint16_t size)
{
	uint16_t out = 0;
	int bits_read = 0, bit_flag;


	// test
	printf("buffer in function %s\n", data);

	/* Sanity check: */
	if (data == NULL)
		return 0;

	while (size > 0)
	{
		bit_flag = out >> 15;

		/* Get next bit: */
		out <<= 1;
		out |= (*data >> bits_read) & 1; // item a) work from the least significant bits

										 /* Increment bit counter: */
		bits_read++;
		if (bits_read > 7)
		{
			bits_read = 0;
			data++;
			size--;
		}

		/* Cycle check: */
		if (bit_flag)
			out ^= CRC16;

	}

	// item b) "push out" the last 16 bits
	int i;
	for (i = 0; i < 16; ++i) {
		bit_flag = out >> 15;
		out <<= 1;
		if (bit_flag)
			out ^= CRC16;
	}

	// item c) reverse the bits
	uint16_t crc = 0;
	i = 0x8000;
	int j = 0x0001;
	for (; i != 0; i >>= 1, j <<= 1) {
		if (i & out) crc |= j;
	}

	return crc;
}
