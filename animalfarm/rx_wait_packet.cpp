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
	long long startTime = generateTimestamp();
	DWORD PACKET_TIMER = static_cast<DWORD>(ceil(8.0 * packetSize / GlobalVar::g_cc.dcb.BaudRate * 1000) * 3);
	PACKET_TIMER = 3000;

	while (generateTimestamp() - startTime < PACKET_TIMER)
	{
		if (ipc_recieve_syn(SYN_TIMER))
		{
			if (ipc_recieve_packet(packet, PACKET_TIMER)) {
				if (rxpp_parse_packet(packet)) {
					return true;//packet recieved
				}
			}
			
			return true;
		}
	}

	LOGMESSAGE(L"Timeout SYN ----------- " << generateTimestamp() << L" ----------- Timeout:" << PACKET_TIMER << std::endl);

	return false;
}
