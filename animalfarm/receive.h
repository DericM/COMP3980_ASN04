#pragma once

bool ipc_recieve_ack(DWORD timeout);
bool ipc_recieve_enq(DWORD timeout);
bool ipc_recieve_packet(char* readchar, DWORD timeout);
bool ipc_read_from_port(char* readChar, DWORD toReadSize, char target, DWORD timeout);
char* checkChar(char readChar);
