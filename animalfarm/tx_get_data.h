#pragma once

#include <string>


uint16_t calculateCRC16(const std::string& data);



bool txgd_setup();
bool txgd_get_next_packet();
