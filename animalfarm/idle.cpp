#include "stdafx.h"
#include "globalvar.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include "idle.h"
#include "tx_wait_connect.h"
#include "rx_connect.h"



//LPCWSTR	lpszCommName;

/*Counters*/
int ENQ_COUNTER;


/*timeouts*/
int RAND_TIMEOUT;
int IDLE_SEQ_TIMEOUT = 500;

/*events*/
//HANDLE hEnqEvent;
OVERLAPPED osReader = { 0 };


/*Flags*/
bool bSendingFile = false;




struct EnqParams
{
	HWND hWnd;
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
void idle_setup(HWND& hWnd, LPCWSTR lpszCommName) {
	LOGMESSAGE(L"\n");
	LOGMESSAGE(L"Entering: idle_setup()\n");

	ENQ_COUNTER = 0;
	idle_rand_timeout_reset();

	try {
		idle_open_port(hWnd, lpszCommName);////////////////////////
	}
	catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("Failed in Idle Setup");
	}

	LOGMESSAGE(L"ENQ_COUNTER assigned value: ");
	LOGMESSAGE(L"" + ENQ_COUNTER + '\n');
	LOGMESSAGE(L"Entering: idle_setup()\n");

	GlobalVar::g_hIdleWaitThread = CreateThread(NULL, 0, idle_wait, (LPVOID)hWnd, 0, 0);
}


/* 
* Open the comm port.
*/
void idle_open_port(HWND& hWnd, LPCWSTR& lpszCommName) {
	LOGMESSAGE(L"Entering: idle_open_port()\n");

	COMMCONFIG cc;
	lpszCommName = L"com5";
	//set comm settings
	cc.dwSize = sizeof(COMMCONFIG);
	cc.wVersion = 0x100;
	GetCommConfig(GlobalVar::g_hComm, &cc, &cc.dwSize);
	if (!CommConfigDialog(lpszCommName, hWnd, &cc)) {
		throw std::runtime_error("Failed to configure Comm Settings");
	}

	//open comm port
	GlobalVar::g_hComm = CreateFile(
		lpszCommName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL
	);

	//check for failure
	if (GlobalVar::g_hComm == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed to open Comm Port");
	}

	SetCommState(GlobalVar::g_hComm, &cc.dcb);

}

/*
* Resets randTimeout to a value between 0-100
*/
void idle_rand_timeout_reset() {
	LOGMESSAGE(L"\n");
	LOGMESSAGE(L"Entering: idle_rand_timeout_reset()\n");

	RAND_TIMEOUT = rand() % 101; //0-100

	LOGMESSAGE(L"RAND_TIMEOUT assigned value: ");
	LOGMESSAGE(L"" + RAND_TIMEOUT + '\n');
}


void idle_go_to_idle() {
	idle_rand_timeout_reset();
	GlobalVar::g_bWaitENQ = TRUE;
	GlobalVar::g_hIdleSendENQThread = CreateThread(NULL, 0, idle_send_enq, &enqParam, 0, 0);
}
/*
IDLE Wait
/////Create listener for transfer button

Create Event for when ENQ is received

Check ENQ counter
	If > 3 Close Threads
		Exit

Start Idle Sequence Timer

If sequence timer times out
	increment Enq counter
	start write thread(starts IDLE Write State)

If transfer button listener triggers(user wants to send file)
	Start / set Rand Timer

If Rand Timer times out
	Stop Sequence Timer
	start write Thread(starts IDLE Write State)

If ENQ received event triggers
	Stop Timers
	start read Thread(starts IDLE Read State)
*/
DWORD WINAPI idle_wait(LPVOID _hWnd) {
	HWND hWnd = (HWND)_hWnd;

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
		enqParam.hWnd = hWnd;
		enqParam.timer = IDLE_SEQ_TIMEOUT;

		GlobalVar::g_hIdleSendENQThread = CreateThread(NULL, 0, idle_send_enq, 0, 0, 0);

		if (ENQ_COUNTER > 3) {
			idle_close_port();
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
						LOGMESSAGE(L"GOT ENQ");
						GlobalVar::g_bWaitENQ = FALSE;
						SetEvent(GlobalVar::g_hEnqEvent);
					}
				}
			}

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
						LOGMESSAGE(L"GOT ENQ");
						GlobalVar::g_bWaitENQ = FALSE;
						SetEvent(GlobalVar::g_hEnqEvent);
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

				idle_create_write_thread(hWnd);
				break;

			default:
				break;
			}
		}
		ResetEvent(osReader.hEvent);
	}

	return 0;
}

DWORD WINAPI idle_send_enq(LPVOID tData_) {

	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hEnqEvent, enqParam.timer);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:

		LOGMESSAGE(L"GOING TO TRANSMISSION");
		GlobalVar::g_bWaitENQ = FALSE;
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
			idle_create_write_thread(tData->hWnd);

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
IDLE Read
Read char
Check if character is ENQ
If character is ENQ stop timers go to Rx Connected State
*/

/*
IDLE Write
Get file name(if Not sequence timer timeout)
Send ENQ
Got to Wait for Connect WFC Wait State
*/
void idle_create_write_thread(HWND& hWnd) {
	LOGMESSAGE(L"\n");
	LOGMESSAGE(L"Entering: idle_create_write_thread\n");

	//make new thread for reading
	GlobalVar::g_hReadThread = CreateThread(
		NULL,
		0,
		write_thread_entry_point,
		(LPVOID)hWnd,
		0,
		NULL
	);
}


DWORD WINAPI write_thread_entry_point(LPVOID pData) {
	LOGMESSAGE(L"\n");
	LOGMESSAGE(L"Entering: write_thread_entry_point\n");
	writeEnqToPort();

	WaitForConnectAck((HWND&)pData, GlobalVar::g_hComm, ENQ_COUNTER);

	return TRUE;

}

/*
* exit the program.
*/
void idle_close_port() {
	LOGMESSAGE(L"Entering: idle_close_port()\n");

	CloseHandle(osReader.hEvent);
	CloseHandle(GlobalVar::g_hComm);
}

bool writeEnqToPort()
{
	LOGMESSAGE(L"Entering: writeEnqToPort()\n");

	OVERLAPPED osWrite = { 0 };
	DWORD dwWritten;
	DWORD dwToWrite = 1;
	bool fRes;
	char ENQ = 0x05;

	// Create this writes OVERLAPPED structure hEvent.
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL)
		// Error creating overlapped event handle.
		return FALSE;

	// Issue write.
	if (!WriteFile(GlobalVar::g_hComm, &ENQ, dwToWrite, &dwWritten, &osWrite)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			// WriteFile failed, but it isn't delayed. Report error and abort.
			fRes = FALSE;
		}
		else {
			// Write is pending.
			if (!GetOverlappedResult(GlobalVar::g_hComm, &osWrite, &dwWritten, TRUE))
				fRes = FALSE;
			else
				// Write operation completed successfully.
				fRes = TRUE;
		}
	}
	else {
		// WriteFile completed immediately.
		fRes = TRUE;
	}

	CloseHandle(osWrite.hEvent);
	return fRes;
}

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
