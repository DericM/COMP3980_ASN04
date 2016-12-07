#include "stdafx.h"
#include "globalvar.h"
#include "rx_wait_packet.h"
#include "rx_parse_packet.h"
#include "receive.h"
#include "packetDefine.h"


bool rxwp_wait_for_packet() {
	
	char packet[HEADER_SIZE + DATA_SIZE + CRC_SIZE + 1];
	

	if (ipc_recieve_packet(packet, GlobalVar::T_RX)) {
		packet[HEADER_SIZE + DATA_SIZE + CRC_SIZE] = '\0';
		LOGMESSAGE(L"rawpacket: " << packet << "\n")
		if (rxpp_parse_packet(packet)) {
			return true;//packet recieved
		}
	}

	return false;//no packet
}
