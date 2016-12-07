/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: recieve.cpp : Handles all the reading from the port for the animal farm protocol
--
-- PROGRAM: Animal Farm
--
-- FUNCTIONS:
-- bool ipc_recieve_ack(DWORD timeout) 
-- bool ipc_recieve_enq(DWORD timeout) 
-- bool ipc_recieve_syn(DWORD timeout) 
-- bool ipc_recieve_packet(char* readChar, DWORD timeout)
-- int ipc_read_from_port(char* readChar, DWORD toReadSize, char target, DWORD timeout, bool bPacket)
----------------------------------------------------------------------------------------------------------------------*/


#include "stdafx.h"
#include "receive.h"
#include "globalvar.h"
#include "packetDefine.h"
#include "testingfunctions.h"

struct ReceiveParams
{
	DWORD    timeout;
	char   target;
	DWORD  toReadSize;
	char * readChar;
	long long startTime;
};
ReceiveParams recieveParam;

int TERMINATE_THREAD_TIMEOUT2 = 500;

HANDLE receiveThread;
HANDLE receiveDataEvent;
bool   f_runningThread;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ipc_recieve_ack
--
-- NOTES:
-- Sets up to read an ACK from the port then calls read_from_port
----------------------------------------------------------------------------------------------------------------------*/
bool ipc_recieve_ack(DWORD timeout) {

	char readChar[1];
	DWORD toReadSize = 1;
	char target = 0x06;
	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Received ACK ----------- " << generateTimestamp() << std::endl);
		return true;
	}
	LOGMESSAGE(L"Timeout ACK ----------- " << generateTimestamp() << L" ----------- Timeout:" << timeout << std::endl);

	return false;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ipc_recieve_enq
--
-- NOTES:
-- Sets up to read an ENQ from the port then calls read_from_port
----------------------------------------------------------------------------------------------------------------------*/
bool ipc_recieve_enq(DWORD timeout) {

	char readChar[1];
	DWORD toReadSize = 1;
	char target = 0x05;

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Received ENQ ----------- " << generateTimestamp() << std::endl);
		return true;
	}
	LOGMESSAGE(L"Timeout ENQ ----------- " << generateTimestamp() << L" ----------- Timeout:" << timeout << std::endl);
	return false;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ipc_recieve_syn
--
-- NOTES:
-- Sets up to read an SYN from the port then calls read_from_port
----------------------------------------------------------------------------------------------------------------------*/
bool ipc_recieve_syn(DWORD timeout) {

	char readChar[1];
	DWORD toReadSize = 1;
	char target = 0x16;

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Received SYN ----------- " << generateTimestamp() << std::endl);
		return true;
	}
	//LOGMESSAGE(L"Timeout SYN ----------- " << generateTimestamp() << L" ----------- Timeout:" << timeout << std::endl);
	return false;
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ipc_recieve_packet
--
-- NOTES:
-- Sets up to read an packet from the port then calls read_from_port
----------------------------------------------------------------------------------------------------------------------*/
bool ipc_recieve_packet(char* readChar, DWORD timeout) {

	DWORD toReadSize = DATA_SIZE + CRC_SIZE;
	char target = 0x16;

	if (ipc_read_from_port(readChar, toReadSize, target, timeout, true)) {
		LOGMESSAGE(L"Received PAC ----------- " << generateTimestamp() << std::endl);
		return true;
	}
	LOGMESSAGE(L"Timeout PAC ----------- " << generateTimestamp() << L" ----------- Timeout:" << timeout << std::endl);
	return false;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ipc_read_from_port
--
-- NOTES:
-- Reads for a specified target fromt he port using events
----------------------------------------------------------------------------------------------------------------------*/
int ipc_read_from_port(char* readChar, DWORD toReadSize, char target, DWORD timeout, bool bPacket) {
	HANDLE& hComm = GlobalVar::g_hComm;

	OVERLAPPED osReader = { 0 };
	DWORD readBytes;

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. " << std::endl);
		return 0;
	}

	BOOL bResult = ReadFile(hComm, readChar, toReadSize, &readBytes, &osReader);
	if (GetLastError() == ERROR_IO_PENDING)
	{
		DWORD dwRes = WaitForSingleObject(osReader.hEvent, timeout);
		switch (dwRes)
		{
		case WAIT_OBJECT_0:
			if (!GetOverlappedResult(hComm, &osReader, &readBytes, FALSE))
			{
				LOGMESSAGE(L"!GetOverlappedResult()" << std::endl);
			}
			else
			{
				if (bPacket || (toReadSize == readBytes && readChar[0] == target))
				{
					LOGMESSAGE(L"(1)RECEVIED CHARACTER : " << (int)readChar[0] << L" ---------- " << generateTimestamp() << std::endl);
					return readBytes;
				}
			}
			break;
		case WAIT_TIMEOUT:
			break;

		default:
			LOGMESSAGE(L"DEFAULT" << std::endl);
			break;
		}

		CancelIo(hComm);
	}
	else if (bResult == TRUE)
	{
		if (!GetOverlappedResult(hComm, &osReader, &readBytes, FALSE))
		{
			LOGMESSAGE(L"!GetOverlappedResult()" << std::endl);
		}
		else
		{
			if (bPacket || (toReadSize == readBytes && readChar[0] == target))
			{
				LOGMESSAGE(L"(2)RECEVIED CHARACTER : " << (int)readChar[0] << L" ---------- " << generateTimestamp() << std::endl);
				return readBytes;
			}
		}
	}

	CloseHandle(osReader.hEvent);

	return 0;

}
