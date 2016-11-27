#include "stdafx.h"
#include "tx_wait_connect.h"

int ENQ_TIMER = 1000;
BOOL WaitForConnectAck(HWND hwnd, HANDLE hcomm, OVERLAPPED reader, int& enqCounter) {
#ifdef _DEBUG
	OutputDebugStringW(L"\n");
	OutputDebugStringW(L"Entering: WaitForConnectAck\n");
#endif
	char readChar;
	BOOL receivedAck = false;
	BOOL fWaitingOnRead = false;
	DWORD eventRet;

	if (reader.hEvent == NULL) {
		//reader is null
		MessageBox(NULL, L"Reader Event is NULL", L"", MB_OK);
		return false;
	}
	if (!fWaitingOnRead) {
		// Issue read operation.
		if (!ReadFile(hcomm, &readChar, 1, &eventRet, &reader)) {
			if (GetLastError() != ERROR_IO_PENDING) {
			} else {
				fWaitingOnRead = TRUE;
			}
				
		}
		else {
			// read completed immediately
			//HandleASuccessfulRead(readChar, hwnd);
		}
	}

	if (fWaitingOnRead) {
		eventRet = WaitForSingleObject(reader.hEvent, ENQ_TIMER);

		switch (eventRet) {
		case WAIT_OBJECT_0:
			if (!GetOverlappedResult(hcomm, &reader, &eventRet, FALSE)) {
				//do something here
			}
			else {
				// Read completed successfully.
				if (readChar == 0x06) {//ACK
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
			// Operation isn't complete yet. fWaitingOnRead flag isn't
			// changed since I'll loop back around, and I don't want
			// to issue another read until the first one finishes.
			//
			// This is a good time to do some background work.
			break;
		}
	}
	
}