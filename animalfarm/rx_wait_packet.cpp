#include "stdafx.h"
#include "globalvar.h"
#include "rx_wait_packet.h"
#include "rx_parse_packet.h"
#include "receive.h"
#include "packetDefine.h"


bool rxwp_wait_for_packet() {
	DWORD packetSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	char packet[HEADER_SIZE + DATA_SIZE + CRC_SIZE];


	if (!ipc_recieve_syn(GlobalVar::RXWP_SYN_TIMER)) {
		return false;//couldnt get sync
	}


	if (ipc_recieve_packet(packet, GlobalVar::RXWP_SYN_TIMER)) {
		LOGMESSAGE(L"rawpacket:" << packet << "\n")
		if (rxpp_parse_packet(packet)) {
			return true;//packet recieved
		}
	}

	

	//char * packet = "";
	//if (ipc_recieve_packet(packet, 2000)) {
	//	if (rxpp_parse_packet(packet)) {
	//		return true;//packet recieved
	//	}
	//}
	return false;//no packet
}
