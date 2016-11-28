#include "stdafx.h"
#include "globalvar.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include "rx_connect.h"
#include "globalvar.h"
#include "rx_wait_packet.h"

int PACKET_TIMER = 100000;
OVERLAPPED osReader = {0};

BOOL rxwp_setup(HANDLE hwnd, HANDLE hcomm) {
	
}


DWORD WINAPI idle_wait(LPVOID _hWnd) {
	HWND hWnd = (HWND)_hWnd;

	rxwp_create_event();


	//GlobalVar::g_hReceivingWaitForACK = CreateThread(NULL, 0, rxwp_check_event, NULL, 0, 0);

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
				if (!ReadFile(hComm, &readChar, 1, &eventRet, &osReader)) {
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
						SetEvent(GlobalVar::g_hEnqEvent);
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
					if (readChar == 0x05) {//ENQ
										   //
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
	}

	return 0;
}



DWORD WINAPI rxwp_check_event(LPVOID tData_) {

	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hEnqEvent, PACKET_TIMER);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		//packet received
		//go to parse packet
		break;

	case WAIT_TIMEOUT:
		//no packet
		//go to Idle wait
		break;

	default:

		break;
	}

	return 0;
}

void rxwp_create_event() {
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