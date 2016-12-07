#include "stdafx.h"
#include "globalvar.h"
#include "send.h"
#include "packetDefine.h"
#include "testingfunctions.h"



bool ipc_send_ack() {
	DWORD dwToWrite = 1;
	char ACK = 0x06;
	if (!ipc_send_data_to_port(&ACK, dwToWrite)) {
		return false;
	}
	LOGMESSAGE(L"Sent ACK ---------- " << generateTimestamp() << std::endl);
	return true;
}

bool ipc_send_enq() {
	DWORD dwToWrite = 1;
	char ENQ = 0x05;
	if (!ipc_send_data_to_port(&ENQ, dwToWrite)) {
		return FALSE;
	}
	LOGMESSAGE(L"Sent ENQ ---------- " << generateTimestamp() << std::endl);
	return TRUE;
}

bool ipc_send_packet(const char* packet) {
	DWORD dwToWrite = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	if (!ipc_send_data_to_port(packet, dwToWrite)) {
		return FALSE;
	}
	LOGMESSAGE(L"Sent SYN ---------- " << generateTimestamp() << std::endl);
	return TRUE;
}



bool ipc_send_data_to_port(const char* data, DWORD dwToWrite) {
	HANDLE& hComm = GlobalVar::g_hComm;
	OVERLAPPED osWrite = { 0 };
	DWORD dwWritten;
	DWORD dwRes;
	bool fRes;
	

	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL) {
		return false;
	}

	PurgeComm(hComm, PURGE_TXABORT);
	PurgeComm(hComm, PURGE_TXCLEAR);

	if (!WriteFile(hComm, data, dwToWrite, &dwWritten, &osWrite)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			fRes = false;
		}
		else {
			if (!GetOverlappedResult(hComm, &osWrite, &dwWritten, TRUE)) {
				fRes = false;
			}
			else {
				// Write is pending.
				dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
				switch (dwRes)
				{
					// OVERLAPPED structure's event has been signaled. 
				case WAIT_OBJECT_0:
					if (!GetOverlappedResult(hComm, &osWrite, &dwWritten, FALSE))
						fRes = false;
					else
						// Write operation completed successfully.
						fRes = true;
					break;

				default:
					// An error has occurred in WaitForSingleObject.
					// This usually indicates a problem with the
					// OVERLAPPED structure's event handle.
					fRes = false;
					
					break;
				}
			}
		}
	}
	else {
		fRes = true;
	}
	CloseHandle(osWrite.hEvent);
	return fRes;
}
