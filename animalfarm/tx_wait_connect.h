#pragma once

#include <string>

BOOL txwc_setup(int &enqCounter, const std::wstring& fileName);
DWORD WINAPI txwc_receive_ack(LPVOID);
DWORD WINAPI tx_wait_ack(LPVOID);
void HandleReceivedAck();