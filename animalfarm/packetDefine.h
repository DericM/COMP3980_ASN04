#pragma once

#define HEADER_SIZE 1
#define DATA_SIZE 5
#define CRC_SIZE 2

uint16_t calculateCRC16(const std::string& data);
std::wstring ExePathW();
std::string ExePathA();
