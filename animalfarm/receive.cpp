#include "stdafx.h"
#include "receive.h"
#include "globalvar.h"
#include "packetDefine.h"
#include "testingfunctions.h"

bool ipc_recieve_ack(DWORD timeout) {

	char readChar[1];
	DWORD toReadSize = 1;
	char target = 0x06;

	int ackCounter = 0;
	while (ackCounter < 3) {
		ackCounter++;
		if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
			LOGMESSAGE(L"Received ACK----Timestamp:" << generateTimestamp() << "\n");
			return true;
		}
	}
	LOGMESSAGE(L"Timeout ACK-----Timestamp:" << generateTimestamp() << L"-----Timeout:" << timeout << "\n");
	return false;
}

bool ipc_recieve_enq(DWORD timeout) {

	char readChar[1];
	DWORD toReadSize = 1;
	char target = 0x05;

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Received ENQ----Timestamp:" << generateTimestamp() << "\n");
		return true;
	}
	LOGMESSAGE(L"Timeout ENQ-----Timestamp:" << generateTimestamp() << L"-----Timeout:" << timeout << "\n");
	return false;
}




bool ipc_recieve_packet(char * readChar, DWORD timeout) {

	DWORD toReadSize =  HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	//char target     = 0x16;

	if (ipc_read_from_port(readChar, toReadSize, NULL, timeout)) {
		LOGMESSAGE(L"Received PACKET-Timestamp:" << generateTimestamp() << "\n");
		return true;
	}
	LOGMESSAGE(L"Timeout PACKET--Timestamp:" << generateTimestamp() << L"-----Timeout:" << timeout << "\n");
	return false;
}


bool ipc_read_from_port(char * readChar, DWORD toReadSize, char target, DWORD timeout) {

	OVERLAPPED osReader = { 0 };
	DWORD eventRet;

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (!ReadFile(GlobalVar::g_hComm, readChar, toReadSize, &eventRet, &osReader)) {
		if (GetLastError() == ERROR_IO_PENDING) {
			DWORD dwRes = WaitForSingleObject(osReader.hEvent, timeout);
			switch (dwRes)
			{
			case WAIT_OBJECT_0:
				if (GetOverlappedResult(GlobalVar::g_hComm, &osReader, &eventRet, FALSE)) {
					if (target == NULL || readChar[0] == target) {
						return true;
					}
				}
				break;
			case WAIT_TIMEOUT:
				CancelIo(GlobalVar::g_hComm);
				break;
		}
	}
	else {
		if (target == NULL || readChar[0] == target) {
			return true;
		}
		LOGMESSAGE(L"READ GARBAGE!!!!!!!!!!!!!!!!!!!!!! " << checkChar(readChar[0]) << "\n");
	}
	ResetEvent(osReader.hEvent);
	return false;
}




/*
bool ipc_read_from_port(char * readChar, DWORD toReadSize, char target, DWORD timeout) {

	BOOL fWaitingOnRead = FALSE;
	OVERLAPPED osReader = { 0 };
	DWORD eventRet;

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return false;
	}

	if (!ReadFile(GlobalVar::g_hComm, readChar, toReadSize, &eventRet, &osReader)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			LOGMESSAGE(L"Error reading from port." << GetLastError << " \n");
		}
		else {
			fWaitingOnRead = TRUE;
		}
	}
	else {

		if (target == NULL || readChar[0] == target) {
			return true;
		}
		LOGMESSAGE(L"READ GARBAGE!!!!!!!!!!!!!!!!!!!!!! " << checkChar(readChar[0]) << "\n");
	}
	if (fWaitingOnRead) {
		DWORD dwRes = WaitForSingleObject(osReader.hEvent, timeout);
		switch (dwRes)
		{
		case WAIT_OBJECT_0:
			//LOGMESSAGE(L"WAIT_OBJECT_0==>");
			if (!GetOverlappedResult(GlobalVar::g_hComm, &osReader, &eventRet, FALSE)) {
				LOGMESSAGE(L"!GetOverlappedResult()");
			}
			else {
				if (target == NULL || readChar[0] == target) {
					//LOGMESSAGE(L"GOT_TARGET2==>");
					return true;
				}
				else {
					LOGMESSAGE(L"READ GARBAGE!!!!!!!!!!!!!!!!!!!!!! " << checkChar(readChar[0]) << "\n");
				}
			}
			fWaitingOnRead = FALSE;
			break;
		case WAIT_TIMEOUT:
			CancelIo(GlobalVar::g_hComm);
			//LOGMESSAGE(L"WAIT_TIMEOUT==>" << target << std::endl);
			break;

		default:
			LOGMESSAGE(L"DEFAULT==>\n");
			break;
		}
	}
	ResetEvent(osReader.hEvent);
	return false;

}

*/





char* checkChar(char readChar) {

	char* mychar = "";
	mychar += readChar;

	switch (readChar) {
	case 0x05:
		return "ENQ";
	case 0x06:
		return "ACK";
	case 0x00:
		return "NULL";
	default:
		return mychar;
	}

}





