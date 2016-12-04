#pragma once

#include <string>

//void getData();
DWORD WINAPI openFile(const HWND *box, LPCWSTR pFile);
std::string makePacket(char buff[]);

uint16_t calculateCRC16(const std::string& data);

std::wstring ExePath();


bool txgd_setup();
bool txgd_get_packets(std::string file);
