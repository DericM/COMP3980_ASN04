#include "stdafx.h"
#include "globalvar.h"
#include "send.h"
#include "packetDefine.h"
#include "testingfunctions.h"



bool ipc_send_ack() {
	DWORD dwToWrite = 1;
	char ACK = 0x06;
	if (!ipc_send_data_to_port(&ACK, dwToWrite)) {
		return FALSE;
	}
	LOGMESSAGE(L"Sent ACK--------Timestamp:" << generateTimestamp() << "\n");
	return TRUE;
}

bool ipc_send_enq() {
	DWORD dwToWrite = 1;
	char ENQ = 0x05;
	if (!ipc_send_data_to_port(&ENQ, dwToWrite)) {
		return FALSE;
	}
	LOGMESSAGE(L"Sent ENQ--------Timestamp:" << generateTimestamp() << "\n");
	return TRUE;
}

bool ipc_send_packet(const char* packet) {
	DWORD dwToWrite = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	if (!ipc_send_data_to_port(packet, dwToWrite)) {
		return FALSE;
	}
	LOGMESSAGE(L"Sent PACKET-----Timestamp:" << generateTimestamp() <<"\n");
	return TRUE;
}



bool ipc_send_data_to_port(const char* data, DWORD dwToWrite) {
	HANDLE& hComm = GlobalVar::g_hComm;
	OVERLAPPED osWrite = { 0 };
	DWORD dwWritten;
	DWORD dwRes;
	bool fRes;
	
	PurgeComm(GlobalVar::g_hComm, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR);

	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL) {
		return FALSE;
	}

	if (!WriteFile(hComm, data, dwToWrite, &dwWritten, &osWrite)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			fRes = FALSE;
		}
		else {
			if (!GetOverlappedResult(hComm, &osWrite, &dwWritten, TRUE)) {
				fRes = FALSE;
			}
			else {
				// Write is pending.
				dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
				switch (dwRes)
				{
					// OVERLAPPED structure's event has been signaled. 
				case WAIT_OBJECT_0:
					if (!GetOverlappedResult(hComm, &osWrite, &dwWritten, FALSE))
						fRes = FALSE;
					else
						// Write operation completed successfully.
						fRes = TRUE;
					break;

				default:
					// An error has occurred in WaitForSingleObject.
					// This usually indicates a problem with the
					// OVERLAPPED structure's event handle.
					fRes = FALSE;
					
					break;
				}
			}
		}
	}
	else {
		fRes = TRUE;
	}
	CloseHandle(osWrite.hEvent);
	return fRes;
}
