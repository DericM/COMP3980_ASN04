#pragma once
#include "stdafx.h"
BOOL rxwp_setUp();
DWORD WINAPI rx_read_for_syn(LPVOID pData_);
DWORD WINAPI rx_wait_syn(LPVOID pData_);
void HandleReceivedSYN();
DWORD WINAPI rx_wait_pack(LPVOID pData_);