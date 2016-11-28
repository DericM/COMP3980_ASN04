#include "stdafx.h"
#include "globalvar.h"
#include "tx_wait_connect.h"
#include "idle.h"

#include <stdexcept>
#include <memory>

int ENQ_TIMER = 1000;

struct ConnectParams
{
	HWND hWnd;
	HANDLE hcomm;
	int enqCounter;
	int timer;
	OVERLAPPED reader;
};
ConnectParams conParam;

struct AckParams
{
	HWND hWnd;
	int timer;
};
AckParams ackParam;

BOOL WaitForConnectAck(HWND& hWnd, HANDLE& hcomm, int& enqCounter) {
	LOGMESSAGE(L"\n");
	LOGMESSAGE(L"Entering: WaitForConnectAck\n");

	OVERLAPPED reader = { 0 };
	reader.hEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL    // object name
	);

	if (reader.hEvent == NULL) {
		throw std::runtime_error("Failed to create event");
	}

	GlobalVar::g_hAckEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL    // object name
	);

	conParam.hWnd = hWnd;
	conParam.hcomm = hcomm;
	conParam.enqCounter = enqCounter;
	conParam.reader = reader;

	ackParam.hWnd = hWnd;
	ackParam.timer = ENQ_TIMER;

	GlobalVar::g_hWaitConnectThread = CreateThread(NULL, 0, tx_wait_connect, &conParam, 0, 0);
	GlobalVar::g_hWaitForACKThread = CreateThread(NULL, 0, tx_wait_ack, &ackParam, 0, 0);
}

DWORD WINAPI tx_wait_connect(LPVOID pData_)
{
	ConnectParams* pData = static_cast<ConnectParams*>(pData_);
	while (true)
	{
		if (GlobalVar::g_bWaitACK)
		{
			char readChar;
			BOOL receivedAck = false;
			BOOL fWaitingOnRead = false;
			DWORD eventRet;

			if (pData->reader.hEvent == NULL) {
				//reader is null
				LOGMESSAGE(NULL, L"Reader Event is NULL", L"", MB_OK);
				return false;
			}
			if (!fWaitingOnRead) {
				// Issue read operation.
				if (!ReadFile(pData->hcomm, &readChar, 1, &eventRet, &pData->reader)) {
					if (GetLastError() != ERROR_IO_PENDING) {
					}
					else {
						fWaitingOnRead = TRUE;
					}

				}
				else {
					// read completed immediately
					if (readChar == 0x06) {//ACK
						SetEvent(GlobalVar::g_hAckEvent);
					}
				}
			}

			if (fWaitingOnRead) {
				eventRet = WaitForSingleObject(pData->reader.hEvent, ENQ_TIMER);

				switch (eventRet) {
				case WAIT_OBJECT_0:
					if (!GetOverlappedResult(pData->hcomm, &pData->reader, &eventRet, FALSE)) {
						//do something here
					}
					else {
						// Read completed successfully.
						if (readChar == 0x06) {//ACK
							SetEvent(GlobalVar::g_hAckEvent);
						}
						else {
							LOGMESSAGE(NULL, L"NON ACK CHARACTER RECEIVED", L"", MB_OK);
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
	}

	return 0;
}

DWORD WINAPI tx_wait_ack(LPVOID pData_)
{
	AckParams* pData = static_cast<AckParams*>(pData_);

	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hAckEvent, pData->timer);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		// Received ack;
		break;

	case WAIT_TIMEOUT:
		// Not receieved ack.
		GlobalVar::g_bWaitACK = FALSE;
		
		idle_go_to_idle();
		break;

	default:
		GlobalVar::g_bWaitACK = FALSE;
		break;
	}

	ResetEvent(GlobalVar::g_hAckEvent);

	return 0;
}
