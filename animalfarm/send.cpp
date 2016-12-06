#include "stdafx.h"
#include "globalvar.h"
#include "send.h"
#include "packetDefine.h"
#include <chrono>

using namespace std::chrono;



bool ipc_send_ack() {
	DWORD dwToWrite = 1;
	char ACK = 0x06;
	if (!ipc_send_data_to_port(&ACK, dwToWrite)) {
		return FALSE;
	}
	int ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		).count() - 1480980000000;
	LOGMESSAGE(L"Send ACK--------" << ms << "\n");
	return TRUE;
}

bool ipc_send_enq() {
	DWORD dwToWrite = 1;
	char ENQ = 0x05;
	if (!ipc_send_data_to_port(&ENQ, dwToWrite)) {
		return FALSE;
	}
	int ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		).count() - 1480980000000;
	LOGMESSAGE(L"Send ENQ--------" << ms << "\n");
	return TRUE;
}

bool ipc_send_packet(const char* packet) {
	DWORD dwToWrite = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	if (!ipc_send_data_to_port(packet, dwToWrite)) {
		return FALSE;
	}
	int ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		).count() - 1480980000000;
	LOGMESSAGE(L"Send PACKET-----" << ms <<"\n");
	return TRUE;
}


/*
	Send data to the port
*/
//bool ipc_send_data_to_port(const char* data, DWORD dwToWrite) {
//	HANDLE& hComm = GlobalVar::g_hComm;
//	OVERLAPPED osWrite = { 0 };
//	DWORD dwWritten;
//	bool fRes;
//
//	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//	if (osWrite.hEvent == NULL) {
//		return FALSE;
//	}
//
//	if (!WriteFile(hComm, data, dwToWrite, &dwWritten, &osWrite)) {
//		if (GetLastError() != ERROR_IO_PENDING) {
//			fRes = FALSE;
//		}
//		else {
//			if (!GetOverlappedResult(hComm, &osWrite, &dwWritten, TRUE)) {
//				fRes = FALSE;
//			}
//			else {
//				fRes = TRUE;
//			}
//		}
//	}
//	else {
//		fRes = TRUE;
//	}
//
//	CloseHandle(osWrite.hEvent);
//	return fRes;
//}

bool ipc_send_data_to_port(const char* data, DWORD dwToWrite) {
	HANDLE& hComm = GlobalVar::g_hComm;
	OVERLAPPED osWrite = { 0 };
	DWORD dwWritten;
	bool fRes;
	DWORD dwRes;

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
