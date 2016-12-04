#pragma once
#include "animalfarm.h"
#include "stdafx.h"


void errorCheck(uint8_t codeword);
uint16_t crcReceive(const uint8_t *data, uint16_t size);
