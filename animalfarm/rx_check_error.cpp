#include "stdafx.h"

#include "animalfarm.h"



//error check after receiving codeword, pass in codeword and size of codeword
uint16_t crcReceive(const std::string& data) {
	static constexpr auto poly = 0x8005;
	auto size = data.size();
	uint16_t out = 0;
	int bits_read = 0;
	bool bit_flag;

	std::vector<char> bytes(data.begin(), data.end());

	int i = 0;
	while (size > 0) {
		bit_flag = (out >> 15) != 0;

		/* Get next bit: */
		// item a) work from the least significant bits
		out = (out << 1) | ((bytes[i] >> bits_read) & 1);

		/* Increment bit counter: */
		if (++bits_read > 7) {
			bits_read = 0;
			i++;
			size--;
		}

		/* Cycle check: */
		if (bit_flag) {
			out ^= poly;
		}
	}

	// item b) "push out" the last 16 bits
	for (int i = 0; i < 16; ++i) {
		out = (out << 1) ^ (poly * ((out >> 15) != 0));
	}

	// item c) reverse the bits
	uint16_t crc = 0;
	for (int i = 0x8000, j = 0x001; i; i >>= 1, j <<= 1) {
		if (i & out) {
			crc |= j;
		}
	}
	return crc;
}

//if remainder == 0000..
//error found and go to wait for packet setup

//else start process thread