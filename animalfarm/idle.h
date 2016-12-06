#pragma once
#include "stdafx.h"

void idle_setup(LPCWSTR lpszCommName);
void idle_connect();
DWORD WINAPI idle_wait(LPVOID na);
void idle_rand_timeout_reset();
bool idle_terminate_thread();