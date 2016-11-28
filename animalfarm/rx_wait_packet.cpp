#include "stdafx.h"
#include "globalvar.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include "rx_connect.h"
#include "globalvar.h"
#include "rx_wait_packet.h"

int PACKET_TIMER = 100000;
bool waitForSYN = true;
OVERLAPPED osReader = {0};

BOOL rxwp_setup(HANDLE _hWnd) {
	HWND hWnd = (HWND)_hWnd;

	rxwp_create_event();


	GlobalVar::g_hReceivingWaitForACK = CreateThread(NULL, 0, rxwp_check_event, NULL, 0, 0);

	LOGMESSAGE(L"Entering: idle_wait loop\n");

	rxwp_readFromPort();

	return true;
}

BOOL rxwp_readFromPort() {

	while (true) {
		if (waitForSYN){
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
						SetEvent(GlobalVar::g_hEnqEvent);
					}
				}
			}

			dwRes = WaitForSingleObject(osReader.hEvent, 10000);
			switch (dwRes)
			{
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(GlobalVar::g_hComm, &osReader, &eventRet, FALSE)) {
					//do something here
				}
				else {
					// Read completed successfully.
					if (readChar == 0x16) {//SYN
										   //
						SetEvent(GlobalVar::g_hRXSynEvent);
					}
					else {
						//SYN not received
					}
				}
				//  Reset flag so that another opertion can be issued.
				fWaitingOnRead = FALSE;

				break;

			case WAIT_TIMEOUT:
				LOGMESSAGE(L"WAIT_TIMEOUT\n");
				//didnt get char from port
				break;

			default:
				break;
			}
		}
	}

	return 0;
}



DWORD WINAPI rxwp_check_event(LPVOID var) {

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
		waitForSYN = false;
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

	GlobalVar::g_hRXSynEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL    // object name
		);

	if (GlobalVar::g_hRXSynEvent == NULL) {
		throw std::runtime_error("Failed to create  WP Packet event");
	}
}