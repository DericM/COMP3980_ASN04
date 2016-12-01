#include "stdafx.h"
#include "receive.h"
#include "globalvar.h"




bool ipc_recieve_ack() {
	char target = 0x06;
	DWORD toReadSize = 1;
	char readChar[1];
	int timeout = 500;//////////////////xxxxxxxxxxxxxxxx//////////////////////////

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Successfuly recieved: " << target);
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
		LOGMESSAGE(L"Successfuly recieved: " << target);
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
		LOGMESSAGE(L"Successfuly recieved: " << target);
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

	bool bWaitEnq = true;
	while (bWaitEnq) {
		LOGMESSAGE(L"BEGIN==>");
		if (!fWaitingOnRead) {
			if (!ReadFile(hComm, readChar, toReadSize, &eventRet, &osReader)) {
				if (GetLastError() != ERROR_IO_PENDING) {
					LOGMESSAGE(L"Error reading from port. ");
				}
				else {
					LOGMESSAGE(L"WAITING_TO_READ==>");
					fWaitingOnRead = TRUE;
				}
			}
			else {

				if (target == NULL || readChar[0] == target) {
					LOGMESSAGE(L"GOT_TARGET1==>");
					bWaitEnq = false;
				}
				LOGMESSAGE(L"GOT_NOTHING1==>");
			}
		}
		if (fWaitingOnRead) {
			dwRes = WaitForSingleObject(osReader.hEvent, timeout);
			switch (dwRes)
			{
			case WAIT_OBJECT_0:
				LOGMESSAGE(L"WAIT_OBJECT_0==>");
				if (!GetOverlappedResult(hComm, &osReader, &eventRet, FALSE)) {
					LOGMESSAGE(L"!GetOverlappedResult()");
				}
				else {
					if (target == NULL || readChar[0] == target) {
						LOGMESSAGE(L"GOT_TARGET2==>");
						bWaitEnq = false;
					}
					else {
						LOGMESSAGE(L"GOT_NOTHING2==>");
					}
				}
				fWaitingOnRead = FALSE;
				break;
			case WAIT_TIMEOUT:
				LOGMESSAGE(L"WAIT_TIMEOUT==>");
				return FALSE;

			default:
				LOGMESSAGE(L"DEFAULT==>\n");
				break;
			}
		}
		ResetEvent(osReader.hEvent);
		LOGMESSAGE(L"END\n");
	}
	return TRUE;

}