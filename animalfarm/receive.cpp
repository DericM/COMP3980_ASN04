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


bool ipc_recieve_ack(DWORD timeout) {

	char readChar[1];
	DWORD toReadSize = 1;
	char target = 0x06;
	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Received ACK ----------- " << generateTimestamp() << std::endl);
		return true;
	}
	LOGMESSAGE(L"Timeout ACK ----------- " << generateTimestamp() << L" ----------- Timeout:" << timeout << std::endl);

	PurgeComm(GlobalVar::g_hComm, PURGE_TXABORT);
	PurgeComm(GlobalVar::g_hComm, PURGE_TXCLEAR);
	PurgeComm(GlobalVar::g_hComm, PURGE_RXABORT);
	PurgeComm(GlobalVar::g_hComm, PURGE_RXCLEAR);

	return false;
}

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



bool ipc_recieve_packet(char* readChar, DWORD timeout) {

	DWORD toReadSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	char target = 0x16;

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Received PAC ----------- " << generateTimestamp() << std::endl);
		return true;
	}
	LOGMESSAGE(L"Timeout PAC ----------- " << generateTimestamp() << L" ----------- Timeout:" << timeout << std::endl);
	return false;
}

bool ipc_read_from_port(char* readChar, DWORD toReadSize, char target, DWORD timeout) {
	HANDLE& hComm = GlobalVar::g_hComm;

	OVERLAPPED osReader = { 0 };
	DWORD readBytes;

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. " << std::endl);
		return false;
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
				if (toReadSize == readBytes && readChar[0] == target)
				{
					LOGMESSAGE(L"(1)RECEVIED CHARACTER : " << (int)readChar[0] << L" ---------- " << generateTimestamp() << std::endl);
					return true;
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
			if (toReadSize == readBytes && readChar[0] == target)
			{
				LOGMESSAGE(L"(1)RECEVIED CHARACTER : " << (int)readChar[0] << L" ---------- " << generateTimestamp() << std::endl);
				return true;
			}
		}
	}

	CloseHandle(osReader.hEvent);

	return false;

}
