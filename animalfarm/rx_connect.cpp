#include "stdafx.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include "send.h"
#include <string>
#include "rx_wait_packet.h"
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")


using namespace std;




DWORD WINAPI send_ack(LPVOID var) {

	ipc_send_ack();
	rxwp_setUp();

	return 0;
}




	