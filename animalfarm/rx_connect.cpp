#include "stdafx.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <string>
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")


using namespace std;


	

void send_ack() {
	int result;
	char ACK = 0x06;

	SOCKET ConnectSocket = INVALID_SOCKET;	//initialize to a non-valid socket first

	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//AF: The Internet Protocol version 4 (IPv4) address family.

																//SOCK_STREAM: A socket type that provides sequenced, reliable, two-way, 
																//connection-based byte streams with an OOB data transmission mechanism. 
	
	if (ConnectSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
	}

	
	result = send(ConnectSocket, &ACK, 1, 0);			//send ACK through socket
	//string result1 = to_string(3);
	//MessageBox(NULL, result1.c_str, L"asdf", MB_OK);
}




	