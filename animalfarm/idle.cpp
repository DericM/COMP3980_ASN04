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



void idle_connect() {
	idleThread = CreateThread(NULL, 0, idle_wait, NULL, 0, 0);
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
		//LOGMESSAGE(L"idle=>");
		if (GlobalVar::g_sending_file) {
			//LOGMESSAGE(L"sendingfile=>");
			idle_rand_timeout_reset();
			timeout = RAND_TIMEOUT;
			if (ipc_recieve_enq(timeout)) {
				//LOGMESSAGE(L"!recieved_enq(rnd)=>");
				rxc_send_ack();
			}
		}
		else {
			//LOGMESSAGE(L"!sendingfile=>");
			timeout = IDLE_SEQ_TIMEOUT;
			if (ipc_recieve_enq(timeout)) {
				//LOGMESSAGE(L"recieved_enq(idle_seq)=>");
				rxc_send_ack();
			}
			else {
				idle_rand_timeout_reset();
				timeout = RAND_TIMEOUT;
				if (ipc_recieve_enq(timeout)) {
					//LOGMESSAGE(L"recieved_enq(rand)=>");
					rxc_send_ack();
				}
			}
		}
		//LOGMESSAGE(L"!recieved_enq(rand)=>");
		ipc_send_enq();
		txwc_receive_ack();
	}
	SetEvent(terminateIdleThreadEvent);
	return 0;
}


bool idle_terminate_thread()
{
	frunningIdleThread = false;

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