#include "stdafx.h"
#include "receive2.h"
#include "globalvar.h"
#include "packetDefine.h"




int TERMINATE_THREAD_TIMEOUT2 = 500;

HANDLE receiveThread;
HANDLE receiveDataEvent;
HANDLE terminateThreadEvent;
bool   f_runningThread;

bool ipc_recieve_ack2(int timeout) {

	receiveDataEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (receiveDataEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return FALSE;
	}

	receiveThread = CreateThread(NULL, 0, recieve_ack_thread, NULL, 0, 0);
	if (receiveThread)
	{
		CloseHandle(receiveThread);
	}

	DWORD dwRes = WaitForSingleObject(receiveDataEvent, timeout);
	ResetEvent(receiveDataEvent);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		return true;
	case WAIT_TIMEOUT:
		ipc_terminate_read_thread();
		return false;
	default:
		LOGMESSAGE("Something bad");
		break;
	}
	return false;
}


DWORD WINAPI recieve_ack_thread(LPVOID timeout) {
	char target = 0x06;
	DWORD toReadSize = 1;
	char readChar[1];

	ipc_read_from_port2(readChar, toReadSize, target, *static_cast<int*>(timeout));
	return 0;
}






void ipc_read_from_port2(char readChar[], DWORD toReadSize, char target, int timeout) {
	HANDLE& hComm = GlobalVar::g_hComm;
	
	OVERLAPPED osReader = { 0 };
	BOOL fWaitingOnRead = FALSE;
	DWORD eventRet;

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

	while (f_runningThread) {
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
					f_runningThread = false;
					SetEvent(receiveDataEvent);
				}
				//LOGMESSAGE(L"GOT_NOTHING1==>");
			}
		}
		if (fWaitingOnRead) {
			DWORD dwRes = WaitForSingleObject(osReader.hEvent, timeout);
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
						f_runningThread = false;
						SetEvent(receiveDataEvent);
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

	SetEvent(terminateThreadEvent);
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
}








