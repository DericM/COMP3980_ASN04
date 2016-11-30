#pragma once

#include <string>

BOOL WaitForConnectAck(HWND& hWnd, HANDLE& hcomm, int &enqCounter, const std::wstring& fileName);
DWORD WINAPI tx_wait_connect(LPVOID);
DWORD WINAPI tx_wait_ack(LPVOID);
void HandleReceivedAck();