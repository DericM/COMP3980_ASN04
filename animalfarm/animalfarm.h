#pragma once

#include "resource.h"
#include "rx_connect.h"
#include <iostream>
#include <exception>
#include <regex>

#define CRC16 0x18005

//functions for tx_Get_Data
bool hasSpecialChars(LPCWSTR pFile);
int readFile(char* buff);
void getFilePosition();

static bool LastPacketAck = true;
static bool FinalPacketSent = false;
static bool FileCreated = false;
static int txFailCounter = 0;
static int posCounter = 0;