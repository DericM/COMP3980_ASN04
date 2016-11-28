#include "stdafx.h"
#include "globalvar.h"
#include "send.h"

/*
	Send an ACK through ipc_send_to_port
*/
bool ipc_send_ack() {
	LOGMESSAGE(L"Entering: ipc_send_ack() -> ");
	DWORD dwToWrite = 1;
	char ACK = 0x06;
	if (!ipc_send_to_port(GlobalVar::g_hComm, &ACK, dwToWrite)) {
		LOGMESSAGE(L"Failed to send ACK.");
		return FALSE;
	}
	return TRUE;
}

/*
	Send an ENQ through ipc_send_to_port
*/
bool ipc_send_enq() {
	LOGMESSAGE(L"Entering: ipc_send_enq() -> ");
	DWORD dwToWrite = 1;
	char ENQ = 0x05;
	if (!ipc_send_to_port(GlobalVar::g_hComm, &ENQ, dwToWrite)) {
		LOGMESSAGE(L"Failed to send ENQ.");
		return FALSE;
	}
	return TRUE;
}

/*
	Send a packet through ipc_send_to_port
*/
bool ipc_send_packet(char packet[1027]) {
	LOGMESSAGE(L"Entering: ipc_send_enq() -> ");
	DWORD dwToWrite = sizeof(packet);
	if (!ipc_send_to_port(GlobalVar::g_hComm, packet, dwToWrite)) {
		LOGMESSAGE(L"Failed to send packet.");
		return FALSE;
	}
	return TRUE;
}


/*
	Send data to the port
*/
bool ipc_send_to_port(HANDLE hComm, char* data, DWORD dwToWrite)
{
	LOGMESSAGE(L"Entering: ipc_send_to_port()\n");
	OVERLAPPED osWrite = { 0 };
	DWORD dwWritten;
	bool fRes;

	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL) {
		return FALSE;
	}

	if (!WriteFile(hComm, &data, dwToWrite, &dwWritten, &osWrite)) {
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