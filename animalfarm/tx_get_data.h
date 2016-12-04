#pragma once

#include "stdafx.h"

DWORD WINAPI openFile(LPCWSTR pFile);

bool txgd_setup();
bool txgd_get_packets(std::string file);

