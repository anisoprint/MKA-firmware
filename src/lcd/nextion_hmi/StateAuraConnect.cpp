/*
 *
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateAuraConnect.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_ACCONFIG,  0,  "acconfig");

	//Network ID
	NexObject _tId = NexObject(PAGE_ACCONFIG,  15,  "tID");
	//Server URL
	NexObject _tSU = NexObject(PAGE_ACCONFIG,  10,  "tSU");
	//Security Code
	NexObject _tSC = NexObject(PAGE_ACCONFIG,  13,  "tSC");


	//Buttons
	NexObject _bCancel = NexObject(PAGE_ACCONFIG,  7,  "bC");
	NexObject _bOk = NexObject(PAGE_ACCONFIG,  6,  "bOK");

	NexObject *_listenList[] = {&_tSU, &_tSC, &_bCancel, &_bOk, NULL};

	char serverURL[45] = { 0 };
	char serverSecurityCode[12] = { 0 };

};

void StateAuraConnect::Init() {
	_bCancel.attachPush(Cancel_Push, &_bCancel);
	_bOk.attachPush(OK_Push, &_bOk);

}


void StateAuraConnect::Activate() {
	NextionHMI::ActivateState(PAGE_CONNECTIONS);
	_page.show();

	char databuffer[64] = {0};
	if (netBridgeManager.GetAcServerId(databuffer, sizeof(databuffer)))
	{
		databuffer[6] = '\0';
		ZERO(NextionHMI::buffer);
		sprintf_P(NextionHMI::buffer, PSTR(MSG_NETWORK_ID), databuffer);
		_tId.setText(NextionHMI::buffer);
	}
	if (strlen(serverURL)>0)
	{
		_tSU.setText(serverURL);
	}
	else
	{
		ZERO(NextionHMI::buffer);
		if (netBridgeManager.GetAcServerUrl(NextionHMI::buffer, sizeof(NEXHMI_BUFFER_SIZE)))
		{
			_tSU.setText(NextionHMI::buffer);
		}
	}
	if (strlen(serverSecurityCode)>0)
	{
		_tSC.setText(serverSecurityCode);
	}


}

void StateAuraConnect::ServerUrl_Push(void *ptr) {
}

void StateAuraConnect::ServerSecurityCode_Push(void *ptr) {
}

void StateAuraConnect::TouchUpdate() {
	nexLoop(_listenList);
}

void StateAuraConnect::Cancel_Push(void *ptr) {
	StateMenu::ActivateNetwork();
}

void StateAuraConnect::OK_Push(void *ptr) {

}



#endif

