/*
 *
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateAuraConnectStatus.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_ACSTATUS,  0,  "acstatus");

	//Network ID
	NexObject _tId = NexObject(PAGE_ACSTATUS,  11,  "tID");
	//Server URL
	NexObject _tSU = NexObject(PAGE_ACSTATUS,  12,  "tSU");
	//Server Connection Status
	NexObject _tSt = NexObject(PAGE_ACSTATUS,  13,  "tSt");

	//Buttons
	NexObject _bConnect = NexObject(PAGE_ACSTATUS,  6,  "bC");
	NexObject _bBack = NexObject(PAGE_ACSTATUS,  7,  "bB");

	NexObject *_listenList[] = {&_bConnect, &_bBack, NULL};

};

void StateAuraConnectStatus::Init() {
	_bConnect.attachPush(Connect_Push, &_bConnect);
	_bBack.attachPush(Back_Push, &_bBack);
}


void StateAuraConnectStatus::Activate() {
	NextionHMI::ActivateState(PAGE_ACSTATUS);
	_page.show();

	char databuffer[64] = {0};
	if (netBridgeManager.GetAcServerId(databuffer, sizeof(databuffer)))
	{
		databuffer[6] = '\0';
		ZERO(NextionHMI::buffer);
		sprintf_P(NextionHMI::buffer, PSTR(MSG_MACHINE_ID), databuffer);
		_tId.setText(NextionHMI::buffer);
	}

	ZERO(NextionHMI::buffer);
	if (netBridgeManager.GetAcServerUrl(NextionHMI::buffer, NEXHMI_BUFFER_SIZE))
	{
		_tSU.setText(NextionHMI::buffer);
	}

	ZERO(NextionHMI::buffer);
	netBridgeManager.GetAcServerStatus(NextionHMI::buffer, NEXHMI_BUFFER_SIZE);
	_tSt.setText(NextionHMI::buffer);

}


void StateAuraConnectStatus::TouchUpdate() {
	nexLoop(_listenList);
}

void StateAuraConnectStatus::Back_Push(void *ptr) {
	StateMenu::ActivateNetwork();
}

void StateAuraConnectStatus::Connect_Push(void *ptr) {
	StateAuraConnect::Activate();
}


#endif

