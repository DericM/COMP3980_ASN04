#pragma once

bool ipc_recieve_ack(DWORD timeout);
bool ipc_recieve_enq(DWORD timeout);
bool ipc_recieve_syn(DWORD timeout);
bool ipc_recieve_packet(char* readchar, DWORD timeout);
void ipc_read_from_port(char* readChar, DWORD toReadSize, char target, DWORD timeout);
DWORD WINAPI recieve_thread(LPVOID na);
bool ipc_terminate_read_thread();

bool createEvents();