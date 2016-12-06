#include "stdafx.h"
#include "globalvar.h"
#include "idle.h"
#include "tx_wait_connect.h"
#include "rx_connect.h"
#include "send.h"
#include "receive.h"
#include "session.h"
#include <random>

/*Counters*/
std::random_device rnd;

/*timeouts*/
int RAND_TIMEOUT;
int IDLE_SEQ_TIMEOUT = 500;
int TERMINATE_THREAD_TIMEOUT = 500;

HANDLE idleThread;
HANDLE terminateIdleThreadEvent;

bool frunningIdleThread;


void idle_setup(LPCWSTR lpszCommName) {

	is_open_port(lpszCommName);
}


/*
* Resets randTimeout to a value between 0-100
*/
void idle_rand_timeout_reset() {

	std::mt19937 rng(rnd());
	std::uniform_int_distribution<int> uni(1, 100);

	RAND_TIMEOUT = uni(rng);
}


//Called after connect button is pushed
void idle_connect() {
	idleThread = CreateThread(NULL, 0, idle_wait, NULL, 0, 0);
	if (idleThread)
		CloseHandle(idleThread);
}






/*
IDLE Wait
*/
DWORD WINAPI idle_wait(LPVOID na) {
	int timeout = IDLE_SEQ_TIMEOUT;
	GlobalVar::g_sending_file = false;
	frunningIdleThread = true;

	terminateIdleThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (terminateIdleThreadEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return 0;
	}

	while (frunningIdleThread) {
		//Sending File timeout Procedure
		if (GlobalVar::g_sending_file) {
			idle_rand_timeout_reset();
			timeout = RAND_TIMEOUT;
			if (ipc_recieve_enq(timeout)) {
				rxc_send_ack();
			}
		}//Idles Sequence Timeout procedure
		else {
			timeout = IDLE_SEQ_TIMEOUT;
			if (ipc_recieve_enq(timeout)) {
				rxc_send_ack();
			}
			else {//After idle Squence timewr times out use rand timout
				idle_rand_timeout_reset();
				timeout = RAND_TIMEOUT;
				if (ipc_recieve_enq(timeout)) {
					rxc_send_ack();
				}
			}
		}
		//Send Enq
		ipc_send_enq();
		//recieve ACK
		txwc_wait_connect_ack();
	}
	//sets event to terminate the thread
	SetEvent(terminateIdleThreadEvent);
	return 0;
}


bool idle_terminate_thread()
{
	frunningIdleThread = false;
	//Waits for Terminate thread event or Terminate_thread Timeout
	DWORD dwRes = WaitForSingleObject(terminateIdleThreadEvent, TERMINATE_THREAD_TIMEOUT);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		return true;

	case WAIT_TIMEOUT:
		TerminateThread(idleThread, 0);
		return false;

	default:
		TerminateThread(idleThread, 0);
		//CloseHandle(hThread);
		return false;
	}
}