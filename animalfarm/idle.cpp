#include "stdafx.h"
#include "globalvar.h"
#include <iostream>
#include <stdexcept>
#include "idle.h"
#include "tx_wait_connect.h"
#include "rx_connect.h"
#include "send.h"
#include "receive.h"
#include "session.h"
#include <random>
#include "tx_get_data.h"

/*Counters*/
std::random_device rnd;

/*timeouts*/
int RAND_TIMEOUT;
int IDLE_SEQ_TIMEOUT = 500;

/*events*/
//OVERLAPPED osReader = { 0 };

/*Flags*/
bool bSendingFile = false;

std::wstring sendFileName = L"";

struct EnqParams
{
	int timer;
};
EnqParams enqParam;


void idle_setup(LPCWSTR lpszCommName) {
	GlobalVar::g_ENQsSent = 0;
	idle_rand_timeout_reset();

	try {
		if (!is_open_port(lpszCommName))
			return;
	}
	catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("Failed in Idle Setup");
	}
}

void idle_go_to_idle() {
	LOGMESSAGE(L"idle_go_to_idle" << std::endl);
	if (GlobalVar::g_hComm == NULL){
		MessageBoxW(GlobalVar::g_hWnd, L"COM setting is not set up yet.", 0, 0);
		return;
	}
	GlobalVar::g_IdleSeq = true;
	idle_rand_timeout_reset();
	sendFileName = L"";

	//TerminateThread(GlobalVar::g_hIdleWaitThread, 0);
	//CloseHandle(GlobalVar::g_hIdleWaitThread);
	GlobalVar::g_hIdleWaitThread = CreateThread(NULL, 0, idle_wait, NULL, 0, 0);
}

/*
* Resets randTimeout to a value between 0-100
*/
void idle_rand_timeout_reset() {

	std::mt19937 rng(rnd());
	std::uniform_int_distribution<int> uni(1, 100);

	RAND_TIMEOUT = uni(rng);
	//RAND_TIMEOUT = rand() % 101; //0-100
}

/*
IDLE Wait
*/
DWORD WINAPI idle_wait(LPVOID pData) {
	int timeout = IDLE_SEQ_TIMEOUT;
	idle_rand_timeout_reset();
	if (GlobalVar::g_SendingFile || !GlobalVar::g_IdleSeq) {
		timeout = RAND_TIMEOUT;
	}

	GlobalVar::g_hEnqEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (GlobalVar::g_hEnqEvent == NULL) {
		throw std::runtime_error("Failed to create event");
	}

	enqParam.timer = IDLE_SEQ_TIMEOUT;

	TerminateThread(GlobalVar::g_hIdleSendENQThread, 0);
	//CloseHandle(GlobalVar::g_hIdleSendENQThread);
	//LOGMESSAGE(L"ENQS Sent " << GlobalVar::g_ENQsSent << "\n");
	/*if (GlobalVar::g_ENQsSent == 3) {
		//LOGMESSAGE(L"SENT ENQS 3 TIMES\nn");
		is_close_port();
		return 0;
	}*/

	if (!ipc_recieve_enq(timeout, &GlobalVar::g_hIdleSendENQThread, idle_send_enq)) {
		//idle_create_write_thread();
	}

	return 0;
}



DWORD WINAPI idle_send_enq(LPVOID tData_) {
	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hEnqEvent, enqParam.timer);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		TerminateThread(GlobalVar::g_hReceivingThread, 0);
		//CloseHandle(GlobalVar::g_hReceivingThread);
		GlobalVar::g_hReceivingThread = CreateThread(NULL, 0, send_ack, NULL, 0, 0);
		break;
	case WAIT_TIMEOUT:
		if (GlobalVar::g_IdleSeq) {
			enqParam.timer = RAND_TIMEOUT;
			idle_send_enq(NULL);
		} 
		else {
			ipc_terminate_read_thread(GlobalVar::g_hIdleWaitThread);
			idle_create_write_thread();
		}
		break;
	default:
		break;
	}

	ResetEvent(GlobalVar::g_hEnqEvent);

	return 0;
}

void idle_create_write_thread() {
	//TerminateThread(GlobalVar::g_hReadThread, 0);
	//CloseHandle(GlobalVar::g_hReadThread);
	GlobalVar::g_hReadThread = CreateThread(
		NULL,
		0,
		write_thread_entry_point,
		NULL,
		0,
		NULL
	);
}


DWORD WINAPI write_thread_entry_point(LPVOID pData) {
	if (ipc_send_enq()) {
		txwc_setup(sendFileName);
	}
	return TRUE;
}


void idle_go_to_sendfile(const LPCWSTR fileName)
{
	bSendingFile = true;
	sendFileName = fileName;
	openFile(&GlobalVar::g_hSendBox, fileName);
}
