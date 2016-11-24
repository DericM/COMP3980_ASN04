#include "stdafx.h"

#include <iostream>
#include <stdexcept>

HANDLE hComm;
LPCWSTR	lpszCommName;

/*Counters*/
int enqCounter;

/*timeouts*/
double randTimeout;
double idleSqeTimeout = 500;

/*events*/
HANDLE hEnqEvent;



/*
Open port
Set Baud rate
Create ENQ counter
Create Rand Timer duration
Create / set Idle Sequence timer duration
Go to IDLE Wait State
*/
void idle_setup(HWND& hWnd) {

	enqCounter = 0;
	idle_rand_timeout_reset();

	try {
		idle_open_port(hWnd, hComm, lpszCommName);
	}
	catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("Failed in Idle Setup");
	}

	idle_wait();
}




void idle_open_port(HWND& hWnd, HANDLE& hComm, LPCWSTR& lpszCommName) {
	COMMCONFIG cc;

	//set comm settings
	cc.dwSize = sizeof(COMMCONFIG);
	cc.wVersion = 0x100;
	GetCommConfig(hComm, &cc, &cc.dwSize);
	if (!CommConfigDialog(lpszCommName, hWnd, &cc)) {
		throw std::runtime_error("Failed to configure Comm Settings");
	}

	//open comm port
	hComm = CreateFile(
		lpszCommName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL
	);

	//check for failure
	if (hComm == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed to open Comm Port");
	}

}


void idle_rand_timeout_reset() {
	randTimeout = rand() % 101; //0-100
}




/*
IDLE Wait
/////Create listener for transfer button

Create Event for when ENQ is received

Check ENQ counter
	If > 3 Close Threads
		Exit

Start Idle Sequence Timer
If sequence timer times out
increment Enq counter
start write thread(starts IDLE Write State)
If transfer button listener triggers(user wants to send file)
Start / set Rand Timer
If Rand Timer times out
Stop Sequence Timer
start write Thread(starts IDLE Write State)
If ENQ received event triggers
Stop Timers
start read Thread(starts IDLE Read State)
*/
void idle_wait() {

	try {
		idle_create_enq_event();

		if (enqCounter > 3) {
			//exit(1);
		}
	}
	catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("Failed in Idle Wait");
	}
}




void idle_create_enq_event() {
	hEnqEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT("ENQEvent")    // object name
	);

	if (hEnqEvent == NULL) {
		throw std::runtime_error("Failed to create Enquire Event");
	}
}





/*
IDLE Read
Read char
Check if character is ENQ
If character is ENQ stop timers go to Rx Connected State
*/






/*
IDLE Write
Get file name(if Not sequence timer timeout)
Send ENQ
Got to Wait for Connect WFC Wait State
*/