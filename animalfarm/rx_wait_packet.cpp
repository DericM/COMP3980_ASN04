#include "stdafx.h"
#include "globalvar.h"
#include "rx_wait_packet.h"
#include "rx_parse_packet.h"
#include "receive.h"
#include "packetDefine.h"


bool rxwp_wait_for_packet() {
	double packetSize = HEADER_SIZE + DATA_SIZE + CRC_SIZE;
	int SYN_TIMER = ceil(packetSize / GlobalVar::g_cc.dcb.BaudRate * 1000) * 3;
	LOGMESSAGE(L"SYN_TIMER:" << SYN_TIMER << " \n");

	char packet[HEADER_SIZE + DATA_SIZE + CRC_SIZE];

	if (ipc_recieve_packet(packet, SYN_TIMER)) {
		if (rxpp_parse_packet(packet)) {
			return true;//packet recieved
		}
	}

	//if (!ipc_recieve_syn(SYN_TIMER)) {
	//	return false;//couldnt get sync
	//}

	//char * packet = "";
	//if (ipc_recieve_packet(packet, 2000)) {
	//	if (rxpp_parse_packet(packet)) {
	//		return true;//packet recieved
	//	}
	//}
	return false;//no packet
}
