#pragma once

#include "resource.h"
#include "rx_connect.h"
#include <iostream>
#include <exception>
#include <regex>

#define CRC16 0x18005

//functions for tx_Get_Data
bool hasSpecialChars(LPCWSTR pFile);
int readFile(OVERLAPPED osReader);
void getFilePosition();
uint8_t* createPacket(char data[]);
uint16_t gen_crc16(const uint8_t *data, uint16_t size);

static bool LastPacketAck = true;
static bool FinalPacketSent = false;
static bool FileCreated = false;
static int txFailCounter = 0;
static int posCounter = 0;