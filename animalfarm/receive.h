#pragma once

bool ipc_recieve_ack(int timeout, HANDLE* hThread, LPTHREAD_START_ROUTINE routine);
bool ipc_recieve_enq(int timeout, HANDLE* hThread, LPTHREAD_START_ROUTINE routine);
bool ipc_recieve_syn(int timeout, HANDLE* hThread, LPTHREAD_START_ROUTINE routine);
bool ipc_recieve_packet(char* readchar, HANDLE* hThread, LPTHREAD_START_ROUTINE routine);

bool ipc_read_from_port(char readChar[], DWORD toReadSize, char target, int timeout, HANDLE* hThread, LPTHREAD_START_ROUTINE routine);

bool ipc_terminate_read_thread(HANDLE& hThread);