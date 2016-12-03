#pragma once

#include <string>

BOOL WaitForConnectAck(const std::wstring& fileName);
DWORD WINAPI tx_wait_connect(LPVOID);
DWORD WINAPI tx_wait_ack(LPVOID);
//void HandleReceivedAck();