/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: rx_wait_Packet.cpp : reads port for a SYN character and a packet
--
-- PROGRAM: Animal Farm
--
-- FUNCTIONS:
-- bool rxwp_wait_for_packet() 
----------------------------------------------------------------------------------------------------------------------*/

#include "stdafx.h"
#include "globalvar.h"
#include "rx_wait_packet.h"
#include "rx_parse_packet.h"
#include "receive.h"
#include "packetDefine.h"
#include "testingfunctions.h"



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: rxwp_wait_for_packet()
--
-- NOTES:
-- Reads from the Port for a SYN by calling ipc_receive_syn function in the recieve.cpp file and if that is successful 
-- the function then reads from the port for the packet by calling ipc_receive_packet function in the receive.cpp file.
-- if either fails function goes back to Idle
----------------------------------------------------------------------------------------------------------------------*/
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
		if (ipc_recieve_syn(SYN_TIMER))
		{
			if (ipc_recieve_packet(packet, PACKET_TIMER))
			{
				if (rxpp_parse_packet(packet)) {
					return true;//packet recieved
				}
			}
		}
		GetSystemTime(&st);
		curTime = st.wMilliseconds;
	}

	LOGMESSAGE(L"Timeout SYN ----------- " << generateTimestamp() << L" ----------- Timeout:" << PACKET_TIMER << std::endl);

	return false;
}
