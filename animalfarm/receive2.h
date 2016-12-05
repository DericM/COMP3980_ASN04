#pragma once
#include "stdafx.h"

bool ipc_recieve_ack2(int timeout);
DWORD WINAPI recieve_ack_thread(LPVOID timeout);
void ipc_read_from_port2(char readChar[], DWORD toReadSize, char target, int timeout);
bool ipc_terminate_read_thread();