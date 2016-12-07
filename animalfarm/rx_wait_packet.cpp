#include "stdafx.h"
#include "globalvar.h"
#include "rx_wait_packet.h"
#include "rx_parse_packet.h"
#include "receive.h"
#include "packetDefine.h"
#include "testingfunctions.h"


bool rxwp_wait_for_packet() {
	DWORD packetSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	char packet[HEADER_SIZE + DATA_SIZE + CRC_SIZE] = { 0 };
	DWORD SYN_TIMER = 500;
	DWORD PACKET_TIMER = static_cast<DWORD>(ceil(8.0 * packetSize / GlobalVar::g_cc.dcb.BaudRate * 1000) * 3);
	PACKET_TIMER = 3000;

	SYSTEMTIME st;
	GetSystemTime(&st);
	WORD startTime = st.wMilliseconds;
	WORD curTime = st.wMilliseconds;

	LOGMESSAGE(L"Wait For SYN ----------- " << generateTimestamp() << std::endl);
	while (curTime - startTime < PACKET_TIMER)
	{
		if (ipc_recieve_packet(packet, PACKET_TIMER))
		{
			if (rxpp_parse_packet(packet)) {
				return true;//packet recieved
			}
		}
		GetSystemTime(&st);
		curTime = st.wMilliseconds;
	}

	LOGMESSAGE(L"Timeout SYN ----------- " << generateTimestamp() << L" ----------- Timeout:" << PACKET_TIMER << std::endl);

	return false;
}
