#pragma once

bool ipc_recieve_ack(int timeout);
bool ipc_recieve_enq(int timeout);
bool ipc_recieve_syn(int timeout);
bool ipc_recieve_packet(char* readchar, int timeout);
void ipc_read_from_port(char* readChar, DWORD toReadSize, char target, int timeout);
DWORD WINAPI recieve_thread(LPVOID na);
bool ipc_terminate_read_thread();