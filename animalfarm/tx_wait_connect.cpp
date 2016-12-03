#include "stdafx.h"
#include "globalvar.h"
#include "tx_wait_connect.h"
#include "idle.h"
#include "tx_get_data.h"
#include "receive.h"

#include <stdexcept>
#include <memory>
#include <math.h>

struct ConnectParams
{
	int enqCounter;
	int timer;
	OVERLAPPED reader;
};
ConnectParams conParam;

struct AckParams
{
	int timer;
	std::wstring filename;
};
AckParams ackParam;

int ACK_TIMER;


BOOL WaitForConnectAck(const std::wstring& fileName) {
	GlobalVar::g_ENQsSent++;
	LOGMESSAGE(L"\nEntering: WaitForConnectAck\n");
	ACK_TIMER = (ceil(16.0 / GlobalVar::g_cc.dcb.BaudRate * 1000));


	/*/
	OVERLAPPED reader = { 0 };
	reader.hEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL    // object name
	);

	if (reader.hEvent == NULL) {
		throw std::runtime_error("Failed to create event");
	}*/
	
	/*
	conParam.enqCounter = enqCounter;
	conParam.reader = reader;
	conParam.timer = ACK_TIMER;
	*/

	GlobalVar::g_hAckEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL    // object name
	);

	ackParam.timer = ACK_TIMER;
	ackParam.filename = fileName;

	//TerminateThread(GlobalVar::g_hWaitConnectThread, 0);
	//TerminateThread(GlobalVar::g_hWaitForACKThread, 0);
	//CloseHandle(GlobalVar::g_hWaitConnectThread);
	//CloseHandle(GlobalVar::g_hWaitForACKThread);
	GlobalVar::g_hWaitConnectThread = CreateThread(NULL, 0, tx_wait_connect, NULL, 0, 0);
	GlobalVar::g_hWaitForACKThread = CreateThread(NULL, 0, tx_wait_ack, NULL, 0, 0);

	return TRUE;
}

DWORD WINAPI tx_wait_connect(LPVOID pData_)
{
	if (!ipc_recieve_ack(ACK_TIMER)) {
		//timeout
	}
	else {
		//success?
	}
	


	/*
	bool bWaitAck = true;
	while (bWaitAck)
	{
		char readChar;
		BOOL receivedAck = false;
		BOOL fWaitingOnRead = false;
		DWORD eventRet;

		if (conParam.reader.hEvent == NULL) {
			//reader is null
			LOGMESSAGE(L"Reader Event is NULL");
			return false;
		}
		if (!fWaitingOnRead) {
			// Issue read operation.
			if (!ReadFile(GlobalVar::g_hComm, &readChar, 1, &eventRet, &conParam.reader)) {
				if (GetLastError() != ERROR_IO_PENDING) {
				}
				else {
					fWaitingOnRead = TRUE;
				}

			}
			else {
				// read completed immediately
				if (readChar == 0x06) {//ACK
					LOGMESSAGE(L"Received ACK.\n");
					bWaitAck = false;
					HandleReceivedAck();
				}
			}
		}

		if (fWaitingOnRead) {
			eventRet = WaitForSingleObject(conParam.reader.hEvent, conParam.timer);

			switch (eventRet) {
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(GlobalVar::g_hComm, &conParam.reader, &eventRet, FALSE)) {
					//do something here
				}
				else {
					// Read completed successfully.
					if (readChar == 0x06) {//ACK
						LOGMESSAGE(L"Received ACK.\n");
						bWaitAck = false;
						HandleReceivedAck();
					}
					else {
						LOGMESSAGE(L"NON ACK CHARACTER RECEIVED");
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

		ResetEvent(conParam.reader.hEvent);
	}
	*/

	return 0;
}

/*
void HandleReceivedAck()
{
	SetEvent(GlobalVar::g_hAckEvent);
}
*/

DWORD WINAPI tx_wait_ack(LPVOID pData_)
{
	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hAckEvent, ackParam.timer);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		LOGMESSAGE(L"RECEIVED : tx_wait_ack" << std::endl);
		if (ackParam.filename.length() == 0)
			idle_go_to_idle();
		else
			GlobalVar::g_ENQsSent = 0;
			openFile(L"FILE");
		// Received ack;
		break;

	case WAIT_TIMEOUT:
		// Not receieved ack.
		LOGMESSAGE(L"TIMEOUT : tx_wait_ack" << std::endl);
		ipc_terminate_read_thread(GlobalVar::g_hWaitConnectThread);
		idle_go_to_idle();
		break;

	default:
		LOGMESSAGE(L"default???" << std::endl);
		break;
	}

	ResetEvent(GlobalVar::g_hAckEvent);

	return 0;
}
