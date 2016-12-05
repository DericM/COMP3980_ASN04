#pragma once
#include "stdafx.h"


bool ipc_send_ack();
bool ipc_send_enq();
bool ipc_send_packet(const char* packet);

bool ipc_send_data_to_port(const char* send, DWORD dwToWrite);