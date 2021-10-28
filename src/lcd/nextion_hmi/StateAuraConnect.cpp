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
	NexObject _tSU = NexObject(PAGE_ACCONFIG,  14,  "tSU");
	//Security Code
	NexObject _tSC = NexObject(PAGE_ACCONFIG,  15,  "tSC");


	//Buttons
	NexObject _bCancel = NexObject(PAGE_ACCONFIG,  7,  "bC");
	NexObject _bOk = NexObject(PAGE_ACCONFIG,  6,  "bOK");

	NexObject *_listenList[] = {&_tSU, &_tSC, &_bCancel, &_bOk, NULL};

	char serverURL[45] = { 0 };
	char serverSecurityCode[12] = { 0 };

};

void StateAuraConnect::Init() {
	_bCancel.attachPush(Close_Push, &_bCancel);
	_bOk.attachPush(OK_Push, &_bOk);
	_tSU.attachPush(ServerUrl_Push, &_tSU);
	_tSC.attachPush(ServerSecurityCode_Push, &_tSC);

}


void StateAuraConnect::Activate() {
	NextionHMI::ActivateState(PAGE_ACCONFIG);
	_page.show();

	char databuffer[64] = {0};
	if (netBridgeManager.GetAcServerId(databuffer, sizeof(databuffer)))
	{
		databuffer[6] = '\0';
		ZERO(NextionHMI::buffer);
		sprintf_P(NextionHMI::buffer, PSTR(MSG_MACHINE_ID), databuffer);
		_tId.setText(NextionHMI::buffer);
	}
	if (strlen(serverURL)>0)
	{
		_tSU.setText(serverURL);
	}
	else
	{
		ZERO(NextionHMI::buffer);
		if (netBridgeManager.GetAcServerUrl(NextionHMI::buffer, NEXHMI_BUFFER_SIZE))
		{
			strncpy(serverURL, NextionHMI::buffer, sizeof(serverURL));
			_tSU.setText(NextionHMI::buffer);
		}
	}
	if (strlen(serverSecurityCode)>0)
	{
		_tSC.setText(serverSecurityCode);
	}


}

void StateAuraConnect::ServerUrl_Push(void *ptr) {
	StateKeyboard::Activate(false, PSTR(MSG_AC_SERVER_URL), serverURL, ServerUrl_OkPush, Dialog_CancelPush);
}

void StateAuraConnect::TouchUpdate() {
	nexLoop(_listenList);
}

void StateAuraConnect::Close_Push(void *ptr) {
	ZERO(serverURL);
	ZERO(serverSecurityCode);
	StateMenu::ActivateNetwork();
}

void StateAuraConnect::OK_Push(void *ptr) {
	if (strlen(serverURL) == 0)
	{
		StateMessage::ActivatePGM_M(MESSAGE_WARNING, NEX_ICON_WARNING, MSG_WARNING, MSG_AC_SERVER_URL_EMPTY, 1, MSG_OK, Dialog_CancelPush, 0, 0, NEX_ICON_DIALOG_WARNING);
		return;
	}
	if (strlen(serverSecurityCode) == 0)
	{
		StateMessage::ActivatePGM_M(MESSAGE_WARNING, NEX_ICON_WARNING, MSG_WARNING, MSG_AC_SERVER_CODE_EMPTY, 1, MSG_OK, Dialog_CancelPush, 0, 0, NEX_ICON_DIALOG_WARNING);
		return;
	}

	StateMessage::ActivatePGM_M(MESSAGE_DIALOG, NEX_ICON_INFO, MSG_AURA_CONNECT, PSTR(MSG_AC_SERVER_UPDATING), 1, PSTR(MSG_PLEASE_WAIT), 0, 0, 0);
	ZERO(NextionHMI::buffer);
	bool result = netBridgeManager.ConnectAcServer(serverURL, serverSecurityCode, NextionHMI::buffer, NEXHMI_BUFFER_SIZE);
	if (result)
	{
		StateMessage::ActivatePGM_M(MESSAGE_DIALOG_OVER, NEX_ICON_INFO, MSG_AURA_CONNECT, PSTR(MSG_AC_SERVER_UPDATED), 1, PSTR(MSG_OK), Close_Push, 0, 0);
		ZERO(serverURL);
		ZERO(serverSecurityCode);
	}
	else
	{
		StateMessage::ActivatePGM_M(MESSAGE_DIALOG_OVER, NEX_ICON_ERROR, MSG_ERROR, NextionHMI::buffer, 1, PSTR(MSG_OK), Dialog_CancelPush, 0, 0, NEX_ICON_DIALOG_ERROR);
	}


}

void StateAuraConnect::ServerSecurityCode_Push(void *ptr) {
	StateEditNumber::Activate(true, serverSecurityCode, ServerSecurityCode_OkPush, Dialog_CancelPush);
}

void StateAuraConnect::ServerUrl_OkPush(void *ptr) {
	ZERO(serverURL);
	StateKeyboard::GetInputToBuffer(serverURL, sizeof(serverURL));
	Activate();
}

void StateAuraConnect::Dialog_CancelPush(void *ptr) {
	Activate();
}

void StateAuraConnect::ServerSecurityCode_OkPush(void *ptr) {
	ZERO(serverSecurityCode);
	StateEditNumber::GetInputToBuffer(serverSecurityCode, sizeof(serverSecurityCode));
	Activate();
}




#endif

