#pragma once

#include <string>

BOOL txwc_setup(const std::wstring& fileName);
DWORD WINAPI txwc_receive_ack(LPVOID);
DWORD WINAPI txwc_receive_ack_event(LPVOID);