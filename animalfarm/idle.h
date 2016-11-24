#pragma once
#include "stdafx.h"



HANDLE hComm;
LPCWSTR	lpszCommName;

/*Counters*/
int enqCounter;

/*timeouts*/
double randTimeout;
double idleSqeTimeout = 500;

/*events*/
HANDLE hEnqEvent;



void idle_setup(HWND& hWnd);
void idle_wait();



void idle_open_port(HWND& hWnd, HANDLE& hComm, LPCWSTR& lpszCommName);
void idle_rand_timeout_reset();
void idle_create_enq_event();