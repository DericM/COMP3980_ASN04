#include "stdafx.h"
#include "receive.h"
#include "globalvar.h"
#include "packetDefine.h"
#include "testingfunctions.h"

bool ipc_recieve_ack(DWORD timeout) {

	char readChar[1];
	DWORD toReadSize = 1;
	char target = 0x06;

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Received ACK----Timestamp:" << generateTimestamp() << "\n");
		return true;
	}
	LOGMESSAGE(L"Timeout ACK-----Timestamp:" << generateTimestamp() << L"-----Timeout:" << timeout << "\n");
	return false;
}

bool ipc_recieve_enq(DWORD timeout) {

	char readChar[1];
	DWORD toReadSize = 1;
	char target = 0x05;

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Received ENQ----Timestamp:" << generateTimestamp() << "\n");
		return true;
	}
	LOGMESSAGE(L"Timeout ENQ-----Timestamp:" << generateTimestamp() << L"-----Timeout:" << timeout << "\n");
	return false;
}


bool ipc_recieve_packet(char * readChar, DWORD timeout) {

	DWORD toReadSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	char target     = 0x16;

	if (ipc_read_from_port(readChar, toReadSize, target, timeout)) {
		LOGMESSAGE(L"Received PACKET-Timestamp:" << generateTimestamp() << "\n");
		return true;
	}
	LOGMESSAGE(L"Timeout PACKET--Timestamp:" << generateTimestamp() << "\n");
	return false;
}



bool ipc_read_from_port(char * readChar, DWORD toReadSize, char target, DWORD timeout) {
	HANDLE& hComm = GlobalVar::g_hComm;

	static BOOL fWaitingOnRead = FALSE;
	OVERLAPPED osReader = { 0 };
	DWORD eventRet;
	bool successfulyReceivedData = false;

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return false;
	}

	//DWORD readFileTimeout = static_cast<DWORD>(ceil(8.0 * toReadSize / GlobalVar::g_cc.dcb.BaudRate * 1000 ));

	if (!ReadFile(hComm, readChar, toReadSize, &eventRet, &osReader)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			LOGMESSAGE(L"Error reading from port." << GetLastError << " \n");
		}
		else {
			//LOGMESSAGE(L"WAITING_TO_READ==>");
			fWaitingOnRead = TRUE;
		}
	}
	else {

		if (target == NULL || readChar[0] == target) {
			//LOGMESSAGE(L"GOT_TARGET1==>");
			return true;
		}
		//LOGMESSAGE(L"GOT_NOTHING1==>");
	}
	if (fWaitingOnRead) {
		DWORD dwRes = WaitForSingleObject(osReader.hEvent, timeout);
		switch (dwRes)
		{
		case WAIT_OBJECT_0:
			//LOGMESSAGE(L"WAIT_OBJECT_0==>");
			if (!GetOverlappedResult(hComm, &osReader, &eventRet, FALSE)) {
				LOGMESSAGE(L"!GetOverlappedResult()");
			}
			else {
				if (target == NULL || readChar[0] == target) {
					//LOGMESSAGE(L"GOT_TARGET2==>");
					return true;
				}
				else {
					//LOGMESSAGE(L"GOT_NOTHING2==>");
				}
			}
			fWaitingOnRead = FALSE;
			break;
		case WAIT_TIMEOUT:
			CancelIo(hComm);
			//LOGMESSAGE(L"WAIT_TIMEOUT==>" << target << std::endl);
			break;

		default:
			LOGMESSAGE(L"DEFAULT==>\n");
			break;
		}
	}
	ResetEvent(osReader.hEvent);
	return false;

}









