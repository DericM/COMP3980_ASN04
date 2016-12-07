/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Idle.cpp : Takes care of the IDle state in the Animal farm protocol
--
-- PROGRAM: Animal Farm
--
-- FUNCTIONS:
-- void idle_setup(LPCWSTR lpszCommName)
-- int idle_rand_timeout() 
-- void idle_connect()
-- DWORD WINAPI idle_wait(LPVOID na) 
-- bool idle_terminate_thread()
----------------------------------------------------------------------------------------------------------------------*/

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

int TERMINATE_THREAD_TIMEOUT = 500;

/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- void idle_setup(LPCWSTR lpszCommName)
--
-- RETURNS: void.
--
-- NOTES:
-- This function opens the commport when the user selects one.
----------------------------------------------------------------------------------------------------------------------*/
void idle_setup(LPCWSTR lpszCommName) {

	is_open_port(lpszCommName);
}


/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- int idle_rand_timeout()
--
-- RETURNS: int.
--
-- NOTES:
-- This function generates a randome timeout.
----------------------------------------------------------------------------------------------------------------------*/
int idle_rand_timeout() {
	std::mt19937 rng(rnd());
	std::uniform_int_distribution<int> uni(1, 100);
	return uni(rng);
}


/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- void idle_connect()
--
-- RETURNS: void.
--
-- NOTES:
-- This function creates a thread for a new instance of the protocall loop.
----------------------------------------------------------------------------------------------------------------------*/
void idle_connect() {
	GlobalVar::g_hIdleThread = CreateThread(NULL, 0, idle_wait, NULL, 0, 0);
	if (GlobalVar::g_hIdleThread)
		CloseHandle(GlobalVar::g_hIdleThread);
}






/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- DWORD WINAPI idle_wait(LPVOID na)
--
-- RETURNS: DWORD.
--
-- NOTES:
-- This function contains the main loop for the program and determins when to start reading and writing.
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI idle_wait(LPVOID na) {
	int timeout = GlobalVar::IDLE_SEQ_TIMEOUT;
	GlobalVar::g_bRunIdle = true;

	GlobalVar::g_hTerminateIdleEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (GlobalVar::g_hTerminateIdleEvent == NULL) {
		LOGMESSAGE(L"Failed to create hEvent. ");
		return 0;
	}

	while (GlobalVar::g_bRunIdle) {
		//if (GlobalVar::g_sending_file)
		if (GlobalVar::g_bWannaSendFile)
		{
			ipc_send_enq();
			txwc_wait_connect_ack();
			GlobalVar::g_sending_file = !GlobalVar::g_sending_file;
		}
		else
		{
			if (ipc_recieve_enq(GlobalVar::IDLE_SEQ_TIMEOUT))
			{
				rxc_send_ack();
			}
			else
			{
				if (ipc_recieve_enq(idle_rand_timeout()))
				{
					rxc_send_ack();
				}

				GlobalVar::g_sending_file = !GlobalVar::g_sending_file;
			}

		}
	}
	//sets event to terminate the thread
	SetEvent(GlobalVar::g_hTerminateIdleEvent);
	return 0;
}



/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- bool idle_terminate_thread()
--
-- RETURNS: bool.
--
-- NOTES:
-- This function terminates the main thread when it is time to quit.
----------------------------------------------------------------------------------------------------------------------*/
bool idle_terminate_thread()
{
	GlobalVar::g_bRunIdle = false;
	//Waits for Terminate thread event or Terminate_thread Timeout
	DWORD dwRes = WaitForSingleObject(GlobalVar::g_hTerminateIdleEvent, INFINITE);
	switch (dwRes)
	{
	case WAIT_OBJECT_0:
		break;

	case WAIT_TIMEOUT:
		TerminateThread(GlobalVar::g_hIdleThread, 0);
		break;

	default:
		TerminateThread(GlobalVar::g_hIdleThread, 0);
		break;
	}

	CloseHandle(GlobalVar::g_hTerminateIdleEvent);

	return true;
}