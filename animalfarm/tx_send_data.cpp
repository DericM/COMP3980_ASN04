#include "stdafx.h"
#include <stdexcept>
#include "GlobalVar.h"

/*
SP Set Up
	Take in Frame
	Go to SP Write
	If Write packet to port fails
		go back to IDLE Set Up
	go to Wait For Ack WFA Wait
*/
bool txsd_send(char frame[1027]) {
	LOGMESSAGE(L"\nEntering: txsd_send() ");

	OVERLAPPED osWrite = { 0 };
	DWORD dwWritten;
	DWORD dwToWrite = 1;
	bool fRes;

	// Create this writes OVERLAPPED structure hEvent.
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL) {
		// Error creating overlapped event handle.
		fRes = FALSE;
	}

	// Issue write.
	if (!WriteFile(GlobalVar::g_hComm, &frame, dwToWrite, &dwWritten, &osWrite)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			// WriteFile failed, but it isn't delayed. Report error and abort.
			fRes = FALSE;
		}
		else {
			// Write is pending.
			if (!GetOverlappedResult(GlobalVar::g_hComm, &osWrite, &dwWritten, TRUE)) {
				fRes = FALSE;
			}
			else {
				// Write operation completed successfully.
				fRes = TRUE;
			}
		}
	}
	else {
		// WriteFile completed immediately.
		fRes = TRUE;
	}

	CloseHandle(osWrite.hEvent);
	return fRes;
}

