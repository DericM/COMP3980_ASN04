#pragma once
#include "stdafx.h"
#include <string>






void idle_setup(LPCWSTR lpszCommName);
//void idle_wait(HWND& hWnd);
DWORD WINAPI idle_wait(LPVOID);
DWORD WINAPI idle_send_enq(LPVOID);
void idle_go_to_idle();


void idle_rand_timeout_reset();
//void idle_create_event();


void idle_create_write_thread();
DWORD WINAPI write_thread_entry_point(LPVOID pData);

void idle_go_to_sendfile(const LPCWSTR fileName);