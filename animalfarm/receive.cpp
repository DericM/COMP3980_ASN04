#include "stdafx.h"
#include "receive.h"
#include "globalvar.h"




bool ipc_recieve_ack() {
	char target = 0x06;
	DWORD toReadSize = 1;
	char readChar[1];
	int timeout = 500;//////////////////xxxxxxxxxxxxxxxx//////////////////////////

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Successfuly recieved: " + target);
		GlobalVar::g_bWaitACK = FALSE;
		SetEvent(GlobalVar::g_hAckEvent);
		return TRUE;
	}
	else {
		return FALSE;
	}
}


bool ipc_recieve_enq(int timeout) {
	char target = 0x05;
	DWORD toReadSize = 1;
	char readChar[1];

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Successfuly recieved: " + target);
		GlobalVar::g_bWaitENQ = FALSE;
		SetEvent(GlobalVar::g_hEnqEvent);
		return TRUE;
	}
	else {
		return FALSE;
	}
}


bool ipc_recieve_snq() {
	char target = 0x16;
	DWORD toReadSize = 1;
	char readChar[1];
	char timeout = 500;/////////////////////////////////////////////////////////

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Successfuly recieved: " + target);
		//GlobalVar::g_bWaitENQ = FALSE;
		SetEvent(GlobalVar::g_hRXSynEvent);
		return TRUE;
	}
	else {
		return FALSE;
	}
}


bool ipc_recieve_packet() {
	char target = NULL;
	DWORD toReadSize = 1026;
	char readChar[1026];
	int timeout = 500;

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Successfuly recieved: packet");
		//GlobalVar::g_bWaitENQ = FALSE;
		//SetEvent(GlobalVar::g_hEnqEvent);
		return TRUE;
	}
	else {
		LOGMESSAGE(L"PACKET TIMED OUT BAD BAD BAD");
		return FALSE;
	}
}







bool ipc_read_from_port(char readChar[], DWORD toReadSize, char target, int timeout) {
	HANDLE& hComm = GlobalVar::g_hComm;
	DWORD dwRes;
	OVERLAPPED osReader = { 0 };
	 
	BOOL fWaitingOnRead = FALSE;
	DWORD eventRet;

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return FALSE;
	}

	if (!fWaitingOnRead) {
		// Issue read operation.
		if (!ReadFile(hComm, readChar, toReadSize, &eventRet, &osReader)) {
			if (GetLastError() != ERROR_IO_PENDING) {
				LOGMESSAGE(L"Error reading from port. ");
			}
			else {
				LOGMESSAGE(L"===================WAITING TO READ=======================\n");
				fWaitingOnRead = TRUE;
			}
		}
		else {
			
			if (target == NULL || readChar[0] == target) { 
				LOGMESSAGE(L"" + target);
				LOGMESSAGE(L"" + readChar[0]);
				LOGMESSAGE(L"===================GOT TARGET1=======================\n");
				return TRUE;
			}
			LOGMESSAGE(L"===================GOT NOTHING=======================\n");
		}
	}

	dwRes = WaitForSingleObject(osReader.hEvent, timeout);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		LOGMESSAGE(L"===================WAIT_OBJECT_0=======================\n");
		if (!GetOverlappedResult(hComm, &osReader, &eventRet, FALSE)) {
			//do something here
		}
		else {
			// Read completed successfully.
			if (target == NULL || readChar[0] == target) {
				LOGMESSAGE(L"===================GOT TARGET2=======================\n");
				return TRUE;
			}
			else {
				//target not received
			}
		}
		//  Reset flag so that another opertion can be issued.
		fWaitingOnRead = FALSE;
		break;

	case WAIT_TIMEOUT:
		LOGMESSAGE(L"===================WAIT_TIMEOUT=======================\n");
		return FALSE;

	default:
		LOGMESSAGE(L"===================DEFAULT=======================\n");
		break;
	}

}