#include "stdafx.h"
#include "globalvar.h"
#include "send.h"
#include "packetDefine.h"

bool ipc_send_ack() {
	DWORD dwToWrite = 1;
	char ACK = 0x06;
	if (!ipc_send_data_to_port(&ACK, dwToWrite)) {
		return FALSE;
	}
	return TRUE;
}

bool ipc_send_enq() {
	DWORD dwToWrite = 1;
	char ENQ = 0x05;
	if (!ipc_send_data_to_port(&ENQ, dwToWrite)) {
		return FALSE;
	}
	return TRUE;
}

bool ipc_send_packet(char* packet) {
	DWORD dwToWrite = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	if (!ipc_send_data_to_port(packet, dwToWrite)) {
		return FALSE;
	}
	return TRUE;
}


/*
	Send data to the port
*/
bool ipc_send_data_to_port(char* data, DWORD dwToWrite) {
	HANDLE& hComm = GlobalVar::g_hComm;
	OVERLAPPED osWrite = { 0 };
	DWORD dwWritten;
	bool fRes;

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
				fRes = TRUE;
			}
		}
	}
	else {
		fRes = TRUE;
	}

	CloseHandle(osWrite.hEvent);
	return fRes;
}