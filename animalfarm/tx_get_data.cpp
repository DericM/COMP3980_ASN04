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

//void generateCRC() {}

