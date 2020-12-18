/*
 * StateStatus2.cpp
 *
 *  Created on: 1 θών. 2018 γ.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateConnections.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_CONNECTIONS,  0,  "netstatus");


	//Wifi status
	NexObject _pWifi = NexObject(PAGE_CONNECTIONS,  11,  "pWifi");
	NexObject _tWifi = NexObject(PAGE_CONNECTIONS,  10,  "tW");
	NexObject _tWifiSignal = NexObject(PAGE_CONNECTIONS,  13,  "tWs");
	NexObject _tWifiIp4 = NexObject(PAGE_CONNECTIONS,  8,  "tWip");
	NexObject _tWifiMac = NexObject(PAGE_CONNECTIONS,  14,  "tWmac");

	//Ethernet status
	NexObject _tEthIp4 = NexObject(PAGE_CONNECTIONS,  15,  "tEip");
	NexObject _tEthMac = NexObject(PAGE_CONNECTIONS,  16,  "tEmac");

	//Buttons
	NexObject _bBack = NexObject(PAGE_CONNECTIONS,  7,  "bC");
	NexObject _bChooseWifi = NexObject(PAGE_CONNECTIONS,  6,  "bW");

	NexObject *_listenList[] = {&_bBack, &_bChooseWifi, NULL};
};

void StateConnections::Init() {
	_bBack.attachPush(Back_Push, &_bBack);
	_bChooseWifi.attachPush(Wifi_Push, &_bChooseWifi);

}

void StateConnections::Activate() {
	NextionHMI::ActivateState(PAGE_CONNECTIONS);
	_page.show();

}

void StateConnections::TouchUpdate() {
	nexLoop(_listenList);
}

void StateConnections::Back_Push(void *ptr) {
}

void StateConnections::Wifi_Push(void *ptr) {
}

#endif

