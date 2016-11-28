#pragma once


BOOL rxwp_setup(HANDLE hwnd);
void rxwp_create_event();
BOOL rxwp_readFromPort();
DWORD WINAPI rxwp_check_event(LPVOID var);