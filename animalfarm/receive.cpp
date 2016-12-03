#include "stdafx.h"
#include "receive.h"
#include "globalvar.h"

int TERMINATE_THREAD_TIMEOUT = 500;



bool ipc_recieve_ack(int timeout) {
	char target = 0x06;
	DWORD toReadSize = 1;
	char readChar[1];

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Successfuly receieved: ACK" << std::endl);
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
		LOGMESSAGE(L"Successfuly receieved: ENQ" << std::endl);
		SetEvent(GlobalVar::g_hEnqEvent);
		return TRUE;
	}
	else {
		return FALSE;
	}
}


bool ipc_recieve_syn(int timeout) {
	char target = 0x16;
	DWORD toReadSize = 1;
	char readChar[1];

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Successfuly receieved: SYN" << std::endl);
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
		LOGMESSAGE(L"Successfuly receieved: packet" << std::endl);
		return TRUE;
	}
	else {
		LOGMESSAGE(L"PACKET TIMED OUT BAD BAD BAD" << std::endl);
		return FALSE;
	}
}







bool ipc_read_from_port(char readChar[], DWORD toReadSize, char target, int timeout) {
	HANDLE& hComm = GlobalVar::g_hComm;
	DWORD dwRes;
	OVERLAPPED osReader = { 0 };
	BOOL fWaitingOnRead = FALSE;
	DWORD eventRet;

	GlobalVar::g_hTerminateThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (GlobalVar::g_hTerminateThreadEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return FALSE;
	}

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return FALSE;
	}

	BOOL bResult = FALSE;
	GlobalVar::g_hRunReadThread = TRUE;
	while (GlobalVar::g_hRunReadThread) {
		//LOGMESSAGE(L"BEGIN==>");
		if (!fWaitingOnRead) {
			if (!ReadFile(hComm, readChar, toReadSize, &eventRet, &osReader)) {
				if (GetLastError() != ERROR_IO_PENDING) {
					LOGMESSAGE(L"Error reading from port. ");
				}
				else {
					//LOGMESSAGE(L"WAITING_TO_READ==>");
					fWaitingOnRead = TRUE;
				}
			}
			else {

				if (target == NULL || readChar[0] == target) {
					LOGMESSAGE(L"GOT_TARGET1==>");
					GlobalVar::g_hRunReadThread = FALSE;
					bResult = TRUE;
				}
				//LOGMESSAGE(L"GOT_NOTHING1==>");
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
						GlobalVar::g_hRunReadThread = FALSE;
						bResult = TRUE;
					}
					else {
						//LOGMESSAGE(L"GOT_NOTHING2==>");
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
		//LOGMESSAGE(L"END\n");
	}

	SetEvent(GlobalVar::g_hTerminateThreadEvent);

	return bResult;

}

bool ipc_terminate_read_thread(HANDLE& hThread)
{
	GlobalVar::g_hRunReadThread = FALSE;

	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hTerminateThreadEvent, TERMINATE_THREAD_TIMEOUT);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		//CloseHandle(hThread);
		return true;

	case WAIT_TIMEOUT:
		TerminateThread(hThread, 0);
		//CloseHandle(hThread);
		return false;

	default:
		TerminateThread(hThread, 0);
		//CloseHandle(hThread);
		return false;
	}
}
