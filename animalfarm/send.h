#pragma once
#include "stdafx.h"


bool ipc_send_ack();
bool ipc_send_enq();
bool ipc_send_packet(char packet[1027]);

bool ipc_send_to_port(HANDLE hComm, char* send, DWORD dwToWrite);