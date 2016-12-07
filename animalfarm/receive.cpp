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


	receiveThread = CreateThread(NULL, 0, recieve_thread, NULL, 0, 0);
	if (receiveThread)
		CloseHandle(receiveThread);

	DWORD dwRes = WaitForSingleObject(receiveDataEvent, timeout);

	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		LOGMESSAGE(L"Received ACK----Timestamp:" << generateTimestamp() << "\n");
		return true;
	case WAIT_TIMEOUT:
		LOGMESSAGE(L"Timeout ACK-----Timestamp:" << generateTimestamp() << L"-----Timeout:" << timeout << "\n");
		ipc_terminate_read_thread();
		return false;
	default:
		LOGMESSAGE("Something bad");
		break;
	}
	CloseHandle(receiveDataEvent);
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

	DWORD toReadSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	char target     = 0x16;

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Received PACKET-Timestamp:" << generateTimestamp() << "\n");
		return true;
	}
	return true;
}




DWORD WINAPI recieve_thread(LPVOID na) {

	ipc_read_from_port(recieveParam.readChar, 
					   recieveParam.toReadSize, 
					   recieveParam.target, 
					   recieveParam.timeout);
	return 0;
}



bool ipc_read_from_port(char * readChar, DWORD toReadSize, char target, DWORD timeout) {
	HANDLE& hComm = GlobalVar::g_hComm;

	OVERLAPPED osReader = { 0 };
	DWORD eventRet;

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return false;
	}

	if (GetLastError() != ERROR_IO_PENDING)
	{
		DWORD dwRes = WaitForSingleObject(osReader.hEvent, timeout);
		switch (dwRes)
		{
		case WAIT_OBJECT_0:
			//LOGMESSAGE(L"WAIT_OBJECT_0==>");
			if (!GetOverlappedResult(hComm, &osReader, &eventRet, FALSE)) {
				LOGMESSAGE(L"!GetOverlappedResult()");
			}
			else {
				if (target == NULL || readChar[0] == target) {
					LOGMESSAGE(L"GOT_TARGET2==>" << (int)target << std::endl);
					SetEvent(receiveDataEvent);
				}
				else {
					//LOGMESSAGE(L"GOT_NOTHING2==>");
				}
			}
			break;
		case WAIT_TIMEOUT:
			//LOGMESSAGE(L"WAIT_TIMEOUT==>" << target << std::endl);
			break;

		default:
			LOGMESSAGE(L"DEFAULT==>\n");
			break;
		}
	}
	else
	{
		if (!ReadFile(hComm, readChar, toReadSize, &eventRet, &osReader))
		{
			if (GetLastError() != ERROR_IO_PENDING) {
				LOGMESSAGE(L"Error reading from port." << GetLastError << " \n");
			}
		}
	}

	CancelIo(hComm);

}


	return true;
}






