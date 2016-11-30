#include "stdafx.h"
#include "globalvar.h"
#include <iostream>
#include <stdexcept>
#include "idle.h"
#include "tx_wait_connect.h"
#include "rx_connect.h"
#include "send.h"
#include "idle_session.h"

/*Counters*/
int ENQ_COUNTER;

/*timeouts*/
int RAND_TIMEOUT;
int IDLE_SEQ_TIMEOUT = 500;

/*events*/
OVERLAPPED osReader = { 0 };

/*Flags*/
bool bSendingFile = false;

std::wstring sendFileName = L"";

struct EnqParams
{
	int timer;
};
EnqParams enqParam;

/*
Open port
Set Baud rate
Create ENQ counter
Create Rand Timer duration
Create / set Idle Sequence timer duration
Go to IDLE Wait State
*/
void idle_setup(LPCWSTR lpszCommName) {
	LOGMESSAGE(L"\n");
	LOGMESSAGE(L"Entering: idle_setup()\n");

	ENQ_COUNTER = 0;
	idle_rand_timeout_reset();

	try {
		if (!is_open_port(lpszCommName))
			return;
	}
	catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("Failed in Idle Setup");
	}
}

void idle_go_to_idle_wait()
{
	if (GlobalVar::g_hComm == NULL)
	{
		MessageBoxW(GlobalVar::g_hWnd, L"COM setting is not set up yet.", 0, 0);
		return;
	}
	LOGMESSAGE(L"ENQ_COUNTER assigned value: ");
	LOGMESSAGE(L"" + ENQ_COUNTER + '\n');
	LOGMESSAGE(L"Entering: idle_setup()\n");

	TerminateThread(GlobalVar::g_hIdleWaitThread, 0);
	GlobalVar::g_hIdleWaitThread = CreateThread(NULL, 0, idle_wait, NULL, 0, 0);
}

/*
* Resets randTimeout to a value between 0-100
*/
void idle_rand_timeout_reset() {
	LOGMESSAGE(L"\nEntering: idle_rand_timeout_reset() ");

	RAND_TIMEOUT = rand() % 101; //0-100

	LOGMESSAGE(L"RAND_TIMEOUT assigned value: ");
	LOGMESSAGE(L"" + RAND_TIMEOUT + '\n');
}

void idle_go_to_idle() {
	idle_rand_timeout_reset();
	GlobalVar::g_bWaitENQ = TRUE;
	bSendingFile = false;
	sendFileName = L"";


	TerminateThread(GlobalVar::g_hIdleSendENQThread, 0);
	GlobalVar::g_hIdleSendENQThread = CreateThread(NULL, 0, idle_send_enq, &enqParam, 0, 0);
}

/*
IDLE Wait
*/
DWORD WINAPI idle_wait(LPVOID pData) {
	LOGMESSAGE(L"\n");
	LOGMESSAGE(L"Entering: idle_wait()\n");
	
	int timeout = IDLE_SEQ_TIMEOUT;
	idle_rand_timeout_reset();
	if (bSendingFile) {
		LOGMESSAGE(L"fSendingFile TRUE\n");
		LOGMESSAGE(L"Switching to RAND_TIMEOUT\n");
		timeout = RAND_TIMEOUT;
	}

	LOGMESSAGE(L"timeout value: ");
	LOGMESSAGE(L"" + timeout + '\n');


	try {
		idle_create_event();

		// Initialize _all_ fields of the struct (malloc won't zero fill)
		enqParam.timer = IDLE_SEQ_TIMEOUT;

		TerminateThread(GlobalVar::g_hIdleSendENQThread, 0);
		GlobalVar::g_hIdleSendENQThread = CreateThread(NULL, 0, idle_send_enq, NULL, 0, 0);

		if (ENQ_COUNTER > 3) {
			CloseHandle(osReader.hEvent);
			is_close_port();

			return 0;
		}
	}
	catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("Failed in Idle Wait");
	}

	if (osReader.hEvent == NULL) {
		throw std::runtime_error("Reader Event is NULL");
	}

	LOGMESSAGE(L"Entering: idle_wait loop\n");

	while (true) {
		if (GlobalVar::g_bWaitENQ)
		{
			DWORD dwRes;

			char readChar;
			BOOL fWaitingOnRead = FALSE;
			DWORD eventRet;

			if (!fWaitingOnRead) {
				// Issue read operation.
				if (!ReadFile(GlobalVar::g_hComm, &readChar, 1, &eventRet, &osReader)) {
					if (GetLastError() != ERROR_IO_PENDING) {
						throw std::runtime_error("Error reading from port.");
					}
					else {
						fWaitingOnRead = TRUE;
					}

				}
				else {
					// read completed immediately
					if (readChar == 0x05) { //ENQ
						LOGMESSAGE(L"GOT ENQ1 \n");
						HandleReceivedEnq();
					}
				}
			}

			if (fWaitingOnRead)
			{
				dwRes = WaitForSingleObject(osReader.hEvent, timeout);
				switch (dwRes)
				{
				case WAIT_OBJECT_0:
					if (!GetOverlappedResult(GlobalVar::g_hComm, &osReader, &eventRet, FALSE)) {
						//do something here
					}
					else {
						// Read completed successfully.
						if (readChar == 0x05) {//ENQ
							LOGMESSAGE(L"GOT ENQ2 \n");
							HandleReceivedEnq();
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

					idle_create_write_thread();
					break;

				default:
					break;
				}
			}
		}
		ResetEvent(osReader.hEvent);
	}

	return 0;
}

void HandleReceivedEnq()
{
	GlobalVar::g_bWaitENQ = FALSE;
	SetEvent(GlobalVar::g_hEnqEvent);
}


DWORD WINAPI idle_send_enq(LPVOID tData_) {
	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hEnqEvent, enqParam.timer);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:

		LOGMESSAGE(L"GOING TO TRANSMISSION \n");
		GlobalVar::g_bWaitENQ = FALSE;
		TerminateThread(GlobalVar::g_hReceivingThread, 0);
		GlobalVar::g_hReceivingThread = CreateThread(NULL, 0, send_ack, NULL, 0, 0);
		break;

	case WAIT_TIMEOUT:
		if (!bSendingFile) {
			LOGMESSAGE(L"SETTING RANDTIMER");
			enqParam.timer = RAND_TIMEOUT;
			bSendingFile = true;
			idle_send_enq(NULL);
		} else {
			GlobalVar::g_bWaitENQ = FALSE;
			idle_create_write_thread();

		}
		break;

	default:
		GlobalVar::g_bWaitENQ = FALSE;
		break;
	}

	ResetEvent(GlobalVar::g_hEnqEvent);

	return 0;
}




void idle_create_event() {
	LOGMESSAGE(L"Entering: idle_create_event()\n");

	osReader.hEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL    // object name
	);

	if (osReader.hEvent == NULL) {
		throw std::runtime_error("Failed to create event");
	}

	GlobalVar::g_hEnqEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL    // object name
	);

	if (GlobalVar::g_hEnqEvent == NULL) {
		throw std::runtime_error("Failed to create event");
	}
}

/*
IDLE Write
Get file name(if Not sequence timer timeout)
Send ENQ
Got to Wait for Connect WFC Wait State
*/
void idle_create_write_thread() {
	LOGMESSAGE(L"\n");
	LOGMESSAGE(L"Entering: idle_create_write_thread\n");

	//make new thread for reading
	TerminateThread(GlobalVar::g_hReadThread, 0);
	GlobalVar::g_hReadThread = CreateThread(
		NULL,
		0,
		write_thread_entry_point,
		NULL,
		0,
		NULL
	);
}


DWORD WINAPI write_thread_entry_point(LPVOID pData) {

	LOGMESSAGE(L"\nEntering: write_thread_entry_point\n");

	ipc_send_enq();

	WaitForConnectAck(ENQ_COUNTER, sendFileName);

	return TRUE;

}



/*
IDLE Read
Read char
Check if character is ENQ
If character is ENQ stop timers go to Rx Connected State
*/

BOOL read_from_port(HANDLE hcomm, OVERLAPPED reader, int timout) {
	char readChar;
	BOOL fWaitingOnRead = false;
	DWORD eventRet;

	if (reader.hEvent == NULL) {
		throw std::runtime_error("Reader Event is NULL");
	}



	if (!fWaitingOnRead) {
		// Issue read operation.
		if (!ReadFile(hcomm, &readChar, 1, &eventRet, &reader)) {
			if (GetLastError() != ERROR_IO_PENDING) {
				throw std::runtime_error("Error reading from port.");
			}
			else {
				fWaitingOnRead = TRUE;
			}

		}
		else {
			// read completed immediately
			//HandleASuccessfulRead(readChar, hwnd);
		}
	}

	if (fWaitingOnRead) {
		eventRet = WaitForSingleObject(reader.hEvent, timout);

		switch (eventRet) {
		case WAIT_OBJECT_0:
			if (!GetOverlappedResult(hcomm, &reader, &eventRet, FALSE)) {
				//do something here
			}
			else {
				// Read completed successfully.
				if (readChar == 0x05) {//ENQ
					//
				}
				else {
					MessageBox(NULL, L"NON ACK CHARACTER RECEIVED", L"", MB_OK);
					//ack not received
				}
			}
			//  Reset flag so that another opertion can be issued.
			fWaitingOnRead = FALSE;
			break;
		case WAIT_TIMEOUT:

			break;
		}
	}

}

void idle_go_to_sendfile(const std::wstring& fileName)
{
	bSendingFile = true;
	sendFileName = fileName;
}
