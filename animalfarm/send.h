#pragma once
#include "stdafx.h"


bool ipc_send_ack();
bool ipc_send_enq();
bool ipc_send_packet(char* packet);

bool ipc_send_data_to_port(char* send, DWORD dwToWrite);