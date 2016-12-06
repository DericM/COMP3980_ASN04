#include "stdafx.h"
#include "receive.h"
#include "globalvar.h"
#include "packetDefine.h"
#include <chrono>

using namespace std::chrono;

struct ReceiveParams
{
	int    timeout;
	char   target;
	DWORD  toReadSize;
	char * readChar;
};
ReceiveParams recieveParam;

int TERMINATE_THREAD_TIMEOUT2 = 500;

HANDLE receiveThread;
HANDLE receiveDataEvent;
HANDLE terminateThreadEvent;
bool   f_runningThread;


bool ipc_recieve_ack(int timeout) {
	
	receiveDataEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (receiveDataEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return FALSE;
	}
	char readChar[1];
	recieveParam.timeout    = timeout;
	recieveParam.toReadSize = 1;
	recieveParam.target     = 0x06;
	recieveParam.readChar   = readChar;


	receiveThread = CreateThread(NULL, 0, recieve_thread, NULL, 0, 0);

	DWORD dwRes = WaitForSingleObject(receiveDataEvent, timeout);
	ResetEvent(receiveDataEvent);
	int ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		).count() - 1480980000000;
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		LOGMESSAGE(L"Received ACK----" << ms << "\n");
		return true;
	case WAIT_TIMEOUT:
		LOGMESSAGE(L"Timeout ACK-----" << ms << "\n");
		ipc_terminate_read_thread();
		return false;
	default:
		LOGMESSAGE("Something bad");
		break;
	}
	return false;
}

bool ipc_recieve_enq(int timeout) {

	receiveDataEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (receiveDataEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return FALSE;
	}

	char readChar[1];
	recieveParam.timeout    = timeout;
	recieveParam.toReadSize = 1;
	recieveParam.target     = 0x05;
	recieveParam.readChar   = readChar;

	receiveThread = CreateThread(NULL, 0, recieve_thread, NULL, 0, 0);

	DWORD dwRes = WaitForSingleObject(receiveDataEvent, timeout);
	ResetEvent(receiveDataEvent);
	int ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		).count() - 1480980000000;
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		LOGMESSAGE(L"Received ENQ----" << ms << "\n");
		return true;
	case WAIT_TIMEOUT:
		LOGMESSAGE(L"Timeout ENQ-----" << ms << "\n");
		ipc_terminate_read_thread();
		return false;
	default:
		LOGMESSAGE("Something bad");
		break;
	}
	return false;
}


bool ipc_recieve_syn(int timeout) {

	receiveDataEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (receiveDataEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return FALSE;
	}

	char readChar[1];
	recieveParam.timeout    = timeout;
	recieveParam.toReadSize = 1;
	recieveParam.target     = 0x16;
	recieveParam.readChar   = readChar;


	receiveThread = CreateThread(NULL, 0, recieve_thread, NULL, 0, 0);

	DWORD dwRes = WaitForSingleObject(receiveDataEvent, timeout);
	ResetEvent(receiveDataEvent);
	int ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		).count() - 1480980000000;
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		LOGMESSAGE(L"Received SYN----" << ms << "\n");
		return true;
	case WAIT_TIMEOUT:
		LOGMESSAGE(L"Timeout SYN-----" << ms << "\n");
		ipc_terminate_read_thread();
		return false;
	default:
		LOGMESSAGE("Something bad");
		break;
	}
	return false;
}


bool ipc_recieve_packet(char * readChar, int timeout) {

	receiveDataEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (receiveDataEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return FALSE;
	}

	recieveParam.timeout    = timeout;
	recieveParam.toReadSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	recieveParam.target     = 0x16;
	recieveParam.readChar   = readChar;


	receiveThread = CreateThread(NULL, 0, recieve_thread, NULL, 0, 0);

	DWORD dwRes = WaitForSingleObject(receiveDataEvent, timeout);
	ResetEvent(receiveDataEvent);
	int ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		).count() - 1480980000000;
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		LOGMESSAGE(L"Received PACKET-" << ms << "\n");
		return true;
	case WAIT_TIMEOUT:
		LOGMESSAGE(L"Timeout PACKET--" << ms << "\n");
		ipc_terminate_read_thread();
		return false;
	default:
		LOGMESSAGE("Something bad");
		break;
	}
	return false;
}


DWORD WINAPI recieve_thread(LPVOID na) {

	ipc_read_from_port(recieveParam.readChar, 
					   recieveParam.toReadSize, 
					   recieveParam.target, 
					   recieveParam.timeout);
	return 0;
}



void ipc_read_from_port(char * readChar, DWORD toReadSize, char target, int timeout) {
	HANDLE& hComm = GlobalVar::g_hComm;

	OVERLAPPED osReader = { 0 };
	BOOL fWaitingOnRead = FALSE;
	DWORD eventRet;
	bool successfulyReceivedData = false;

	terminateThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (terminateThreadEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return;
	}

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return;
	}

	f_runningThread = true;
	while (f_runningThread) {
		//LOGMESSAGE(L"BEGIN==>");
		if (!fWaitingOnRead) {
			if (!ReadFile(hComm, readChar, toReadSize, &eventRet, &osReader)) {
				if (GetLastError() != ERROR_IO_PENDING) {
					LOGMESSAGE(L"Error reading from port." << GetLastError << " \n");
				}
				else {
					//LOGMESSAGE(L"WAITING_TO_READ==>");
					fWaitingOnRead = TRUE;
				}
			}
			else {

				if (target == NULL || readChar[0] == target) {
					//LOGMESSAGE(L"GOT_TARGET1==>");
					f_runningThread = false;
				}
				//LOGMESSAGE(L"GOT_NOTHING1==>");
			}
		}
		if (fWaitingOnRead) {
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
						LOGMESSAGE(L"GOT_TARGET2==>");
						f_runningThread = false;
					}
					else {
						//LOGMESSAGE(L"GOT_NOTHING2==>");
					}
				}
				fWaitingOnRead = FALSE;
				break;
			case WAIT_TIMEOUT:
				LOGMESSAGE(L"WAIT_TIMEOUT==>");
				break;

			default:
				LOGMESSAGE(L"DEFAULT==>\n");
				break;
			}


		}
		ResetEvent(osReader.hEvent);
		//LOGMESSAGE(L"END\n");
	}
	if (successfulyReceivedData) {
		SetEvent(receiveDataEvent);
	}

	SetEvent(terminateThreadEvent);
}



bool ipc_terminate_read_thread()
{
	f_runningThread = false;

	DWORD dwRes = WaitForSingleObject(terminateThreadEvent, TERMINATE_THREAD_TIMEOUT2);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		//CloseHandle(hThread);
		return true;

	case WAIT_TIMEOUT:
		TerminateThread(receiveThread, 0);
		//CloseHandle(hThread);
		return false;

	default:
		TerminateThread(receiveThread, 0);
		//CloseHandle(hThread);
		return false;
	}
}








