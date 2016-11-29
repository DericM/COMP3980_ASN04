#pragma once

BOOL WaitForConnectAck(HWND& hWnd, HANDLE& hcomm, int &enqCounter);
DWORD WINAPI tx_wait_connect(LPVOID);
DWORD WINAPI tx_wait_ack(LPVOID);
void HandleReceivedAck();