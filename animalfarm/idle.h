#pragma once
#include "stdafx.h"






void idle_setup(HWND& hWnd, LPCWSTR lpszCommName);
//void idle_wait(HWND& hWnd);
DWORD WINAPI idle_wait(LPVOID);
DWORD WINAPI idle_send_enq(LPVOID);



void idle_open_port(HWND& hWnd, LPCWSTR& lpszCommName);
void idle_close_port();

void idle_rand_timeout_reset();
void idle_create_event();

void idle_create_write_thread(HWND& hWnd);
DWORD WINAPI write_thread_entry_point(LPVOID pData);
bool writeEnqToPort();