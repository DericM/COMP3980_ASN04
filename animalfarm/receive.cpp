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
	
	createEvents();

	char readChar[1];
	recieveParam.timeout    = timeout;
	recieveParam.toReadSize = 1;
	recieveParam.target     = 0x06;
	recieveParam.readChar   = readChar;
	recieveParam.startTime = generateTimestamp();


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

	createEvents();

	char readChar[1];
	recieveParam.timeout    = timeout;
	recieveParam.toReadSize = 1;
	recieveParam.target     = 0x05;
	recieveParam.readChar   = readChar;
	recieveParam.startTime = generateTimestamp();

	receiveThread = CreateThread(NULL, 0, recieve_thread, NULL, 0, 0);
	if (receiveThread)
		CloseHandle(receiveThread);

	DWORD dwRes = WaitForSingleObject(receiveDataEvent, timeout);
	ResetEvent(receiveDataEvent);

	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		LOGMESSAGE(L"Received ENQ----Timestamp:" << generateTimestamp() << "\n");
		return true;
	case WAIT_TIMEOUT:
		LOGMESSAGE(L"Timeout ENQ-----Timestamp:" << generateTimestamp() << L"-----Timeout:" << timeout << "\n");
		ipc_terminate_read_thread();
		return false;
	default:
		LOGMESSAGE("Something bad");
		break;
	}
	return false;
}

/*
bool ipc_recieve_syn(DWORD timeout) {

	createEvents();

	char readChar[1];
	recieveParam.timeout    = timeout;
	recieveParam.toReadSize = 1;
	recieveParam.target     = 0x16;
	recieveParam.readChar   = readChar;


	receiveThread = CreateThread(NULL, 0, recieve_thread, NULL, 0, 0);

	DWORD dwRes = WaitForSingleObject(receiveDataEvent, timeout);
	ResetEvent(receiveDataEvent);
	long long ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		).count() - 1480980000000;
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		LOGMESSAGE(L"Received SYN----Timestamp:" << ms << "\n");
		return true;
	case WAIT_TIMEOUT:
		LOGMESSAGE(L"Timeout SYN-----Timestamp:" << ms << "\n");
		ipc_terminate_read_thread();
		return false;
	default:
		LOGMESSAGE("Something bad");
		break;
	}
	return false;
}
*/

bool ipc_recieve_packet(char * readChar, DWORD timeout) {

	createEvents();

	recieveParam.timeout    = timeout;
	recieveParam.toReadSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	recieveParam.target     = 0x16;
	recieveParam.readChar   = readChar;


	receiveThread = CreateThread(NULL, 0, recieve_thread, NULL, 0, 0);
	if (receiveThread)
		CloseHandle(receiveThread);

	DWORD dwRes = WaitForSingleObject(receiveDataEvent, timeout);
	ResetEvent(receiveDataEvent);

	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		LOGMESSAGE(L"Received PACKET-Timestamp:" << generateTimestamp() << "\n");
		return true;
	case WAIT_TIMEOUT:
		LOGMESSAGE(L"Timeout PACKET--Timestamp:" << generateTimestamp() << "\n");
		ipc_terminate_read_thread();
		return false;
	default:
		LOGMESSAGE("Something bad");
		break;
	}
	return false;
}


bool createEvents() {
	receiveDataEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (receiveDataEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return false;
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



void ipc_read_from_port(char * readChar, DWORD toReadSize, char target, DWORD timeout) {
	HANDLE& hComm = GlobalVar::g_hComm;

	OVERLAPPED osReader = { 0 };
	DWORD eventRet;

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return;
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

	CloseHandle(osReader.hEvent);
}



bool ipc_terminate_read_thread()
{
	f_runningThread = false;

	//DWORD dwRes = WaitForSingleObject(terminateThreadEvent, TERMINATE_THREAD_TIMEOUT2);
	//switch (dwRes)
	//{
	//case WAIT_OBJECT_0:
	//	//CloseHandle(hThread);
	//	return true;

	//case WAIT_TIMEOUT:
	//	TerminateThread(receiveThread, 0);
	//	//CloseHandle(hThread);
	//	return false;

	//default:
	//	TerminateThread(receiveThread, 0);
	//	//CloseHandle(hThread);
	//	return false;
	//}

	return true;
}






