#pragma once

BOOL WaitForConnectAck(HWND hwnd, HANDLE hcomm, OVERLAPPED reader, int &enqCounter);