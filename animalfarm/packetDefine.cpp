#include "stdafx.h"
#include "packetDefine.h"
#include <vector>


/*
* Copied and modified from:
* https://stackoverflow.com/questions/21939392/crc-16-program-to-calculate-check-sum
*/
uint16_t calculateCRC16(const std::string& data) {
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

std::wstring ExePathW()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

std::string ExePathA()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}