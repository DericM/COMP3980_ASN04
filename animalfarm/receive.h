#pragma once

bool ipc_recieve_ack();
bool ipc_recieve_enq(int timeout);
bool ipc_recieve_snq();
bool ipc_recieve_packet();

bool ipc_read_from_port(char readChar[], char target, int timeout);