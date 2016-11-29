#include "stdafx.h"
#include "globalvar.h"




bool ipc_recieve_ack() {
	char target = 0x06;
	char readChar[1];
	int timeout = 500;//////////////////xxxxxxxxxxxxxxxx//////////////////////////

	if (ipc_read_from_port(readChar, target, timeout)) {
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
	char readChar[1];

	if (ipc_read_from_port(readChar, target, timeout)) {
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
	char readChar[1];
	char timeout = 500;/////////////////////////////////////////////////////////

	if (ipc_read_from_port(readChar, target, timeout)) {
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
	char readChar[1027];
	int timeout = 500;

	if (ipc_read_from_port(readChar, target, timeout)) {
		LOGMESSAGE(L"Successfuly recieved: packet");
		//GlobalVar::g_bWaitENQ = FALSE;
		//SetEvent(GlobalVar::g_hEnqEvent);
		return TRUE;
	}
	else {
		return FALSE;
	}
}







bool ipc_read_from_port(char readChar[], char target, int timeout) {
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
		if (!ReadFile(hComm, &readChar, sizeof(readChar), &eventRet, &osReader)) {
			if (GetLastError() != ERROR_IO_PENDING) {
				LOGMESSAGE(L"Error reading from port. ");
			}
			else {
				fWaitingOnRead = TRUE;
			}
		}
		else {
			// read completed immediately
			if (target == NULL || readChar[0] == target) { 
				return TRUE;
			}
		}
	}

	dwRes = WaitForSingleObject(osReader.hEvent, timeout);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		if (!GetOverlappedResult(hComm, &osReader, &eventRet, FALSE)) {
			//do something here
		}
		else {
			// Read completed successfully.
			if (target == NULL || readChar[0] == target) {
				return TRUE;
			}
			else {
				//ack not received
			}
		}
		//  Reset flag so that another opertion can be issued.
		fWaitingOnRead = FALSE;
		break;

	case WAIT_TIMEOUT:
		LOGMESSAGE(L"WAIT_TIMEOUT\n");
		return FALSE;

	default:
		LOGMESSAGE(L"Default Case(This should never happen)\n");
		break;
	}

}