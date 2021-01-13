/*
 * StateFiles.cpp
 *
 *  Created on: 7 θών. 2018 γ.
 *      Author: Azarov
 */

#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateWifiList.h"


namespace {

	///////////// Nextion components //////////
	//Page
	NexObject _page  = NexObject(PAGE_WIFILIST,  0,  "wifilist");

	//Buttons
	NexObject _tName0 = NexObject(PAGE_WIFILIST,  21,  "f0");
	NexObject _tName1 = NexObject(PAGE_WIFILIST,  22,  "f1");
	NexObject _tName2 = NexObject(PAGE_WIFILIST,  23,  "f2");
	NexObject _tName3 = NexObject(PAGE_WIFILIST,  24,  "f3");
	NexObject _tName4 = NexObject(PAGE_WIFILIST,  25,  "f4");
	NexObject _tName5 = NexObject(PAGE_WIFILIST,  26,  "f5");
	NexObject _tName6 = NexObject(PAGE_WIFILIST,  27,  "f6");

	NexObject _tIcon0 = NexObject(PAGE_WIFILIST,  10,  "i0");
	NexObject _tIcon1 = NexObject(PAGE_WIFILIST,  12,  "i1");
	NexObject _tIcon2 = NexObject(PAGE_WIFILIST,  14,  "i2");
	NexObject _tIcon3 = NexObject(PAGE_WIFILIST,  28,  "i3");
	NexObject _tIcon4 = NexObject(PAGE_WIFILIST,  29,  "i4");
	NexObject _tIcon5 = NexObject(PAGE_WIFILIST,  30,  "i5");
	NexObject _tIcon6 = NexObject(PAGE_WIFILIST,  31,  "i6");

	NexObject _bUp = NexObject(PAGE_WIFILIST,  32,  "up");
	NexObject _bDown = NexObject(PAGE_WIFILIST,  33,  "dn");

	NexObject _tLoading = NexObject(PAGE_WIFILIST,  34,  "tL");

	NexObject _bUpIcon = NexObject(PAGE_WIFILIST,  17,  "pUP");
	NexObject _bDownIcon = NexObject(PAGE_WIFILIST,  18,  "pDN");

	NexObject _bCancel = NexObject(PAGE_WIFILIST,  9,  "cc");

	NexObject *_listenList[] = { &_tName0, &_tName1, &_tName2, &_tName3, &_tName4, &_tName5, &_tName6,
			&_bUp, &_bDown, &_bCancel, NULL };

	uint32_t _listPosition = 0;
	bool _insideDir = false;

	void WifiList_PopulateRow(uint32_t row, bool isSecure, uint8_t signal, const char* name) {
		NexObject* rowText;
		NexObject* rowIcon;

		switch(row) {
		    case 0 :
		    	 rowText = &_tName0;
		    	 rowIcon = &_tIcon0;
		         break;
		    case 1 :
		    	 rowText = &_tName1;
		    	 rowIcon = &_tIcon1;
		         break;
		    case 2 :
		    	 rowText = &_tName2;
		    	 rowIcon = &_tIcon2;
		         break;
		    case 3 :
		    	 rowText = &_tName3;
		    	 rowIcon = &_tIcon3;
		         break;
		    case 4 :
		    	 rowText = &_tName4;
		    	 rowIcon = &_tIcon4;
		         break;
		    case 5 :
		    	 rowText = &_tName5;
		    	 rowIcon = &_tIcon5;
		         break;
		    case 6 :
		    	 rowText = &_tName6;
		    	 rowIcon = &_tIcon6;
		         break;
		}

		if (name == "")
		{
			rowText->detachPush();
			rowIcon->setPic(NEX_ICON_FILE_BLANK);
			rowText->setText(name);
		}
		else
		{
			rowText->attachPush(StateWifiList::Network_Push, rowText);

			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR(MSG_WIFILISTNAME), name, signal);
			rowText->setText(NextionHMI::buffer);

		    if (isSecure) {
		    	rowIcon->setPic(NEX_ICON_WIFI_SEC_S);
		    }
		    else
		    {
		    	rowIcon->setPic(NEX_ICON_WIFI_S);
		    }

		}
	}

	void WifiList_PopulateList(uint32_t beginIndex) {

		netBridgeManager.ListWifiNetworks(beginIndex);
		uint8_t count = netBridgeManager.networksCount;
		if (count==0) {
			StateWifiList::Up_Push(nullptr);
		}

	    for (uint8_t row = 0; row < 7; row++) {
	      if (row < count) {
			  WifiList_PopulateRow(row, netBridgeManager.wifiNetworks[row].secure, netBridgeManager.wifiNetworks[row].signal, netBridgeManager.wifiNetworks[row].ssid);
	      }
	      else
	      {
	    	  WifiList_PopulateRow(row, false, 0, "");
	      }
	    }

	    if (beginIndex>0)
	    	_bUpIcon.setPic(NEX_ICON_FILES_UP);
	    else
	    	_bUpIcon.setPic(NEX_ICON_FILES_UP_GREY);

	    if (count<7)
	    	_bDownIcon.setPic(NEX_ICON_FILES_DOWN_GREY);
	    else
	    	_bDownIcon.setPic(NEX_ICON_FILES_DOWN);


	}

}

void StateWifiList::Up_Push(void* ptr) {
	if (_listPosition>0)
	{
		_listPosition -= 7;
		if (_listPosition<0) _listPosition=0;
		WifiList_PopulateList(_listPosition);
	}
}

void StateWifiList::Down_Push(void* ptr) {
    _listPosition += 7;
    WifiList_PopulateList(_listPosition);
}

void StateWifiList::Network_Push(void* ptr) {
    ZERO(NextionHMI::buffer);

    uint8_t selectedIndex = 0;

    if (ptr == &_tName0)
    	selectedIndex = 0;
    else if (ptr == &_tName1)
    	selectedIndex = 1;
    else if (ptr == &_tName2)
    	selectedIndex = 2;
    else if (ptr == &_tName3)
    	selectedIndex = 3;
    else if (ptr == &_tName4)
    	selectedIndex = 4;
    else if (ptr == &_tName5)
    	selectedIndex = 5;
    else if (ptr == &_tName6)
    	selectedIndex = 6;

    if (selectedIndex >= netBridgeManager.networksCount) return;
    netBridgeManager.SelectWifiNetwork(netBridgeManager.wifiNetworks[selectedIndex].ssid);
    if (netBridgeManager.wifiNetworks[selectedIndex].secure)
    {
    	StateKeyboard::Activate(true, PSTR(MSG_WIFI_SECURITY_KEY), "", NetKey_OkPush, NetKey_CancelPush);
    }
    else
    {
    	StateMessage::ActivatePGM_M(MESSAGE_DIALOG, NEX_ICON_INFO, MSG_WIFI, MSG_CONNECTING, 1, MSG_PLEASE_WAIT, 0, 0, 0);

    	bool result = netBridgeManager.ConnectWifi(netBridgeManager.GetSelectedWifiNetwork(), "\"\"", NextionHMI::buffer, NEXHMI_BUFFER_SIZE);
    	if (result)
    	{
    		netBridgeManager.UpdateWifiConnected(true);
    		netBridgeManager.UpdateConnectedNetwork();
    		StateConnections::Activate();
    	}
    	else
    	{
    		StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, MSG_ERROR, NextionHMI::buffer, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
    	}
    }


    /*if (strlen(NextionHMI::buffer)>50)
	{
    	StateMessage::ActivatePGM_M(MESSAGE_WARNING, NEX_ICON_WARNING, MSG_WARNING, MSG_LONG_FILENAME, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
    	return;
	}*/

}

void StateWifiList::Cancel_Push(void* ptr) {
	StateConnections::Activate();
}

void StateWifiList::NetKey_OkPush(void* ptr) {
	char passwordBuffer[64] = {0};

	StateKeyboard::GetInputToBuffer(passwordBuffer, sizeof(passwordBuffer));
	StateMessage::ActivatePGM_M(MESSAGE_DIALOG, NEX_ICON_INFO, MSG_WIFI, MSG_CONNECTING, 1, MSG_PLEASE_WAIT, 0, 0, 0);

	bool result = netBridgeManager.ConnectWifi(netBridgeManager.GetSelectedWifiNetwork(), passwordBuffer, NextionHMI::buffer, NEXHMI_BUFFER_SIZE);
	if (result)
	{
		netBridgeManager.UpdateWifiConnected(true);
		netBridgeManager.UpdateConnectedNetwork();
		StateConnections::Activate();
	}
	else
	{
		StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, MSG_ERROR, NextionHMI::buffer, 1, PSTR(MSG_OK), Cancel_Push, 0, 0);
	}
}

void StateWifiList::NetKey_CancelPush(void* ptr) {
	StateWifiList::Activate();
}

void StateWifiList::Init() {
	_bCancel.attachPush(Cancel_Push);
	_bUp.attachPush(Up_Push);
	_bDown.attachPush(Down_Push);
}

void StateWifiList::Activate() {
    _listPosition = 0;
    NextionHMI::ActivateState(PAGE_WIFILIST);
    _page.show();
    if (netBridgeManager.ScanWifiNetworks())
    {
    	printer.safe_delay(4000);
    }
    WifiList_PopulateList(_listPosition);
	_tLoading.SetVisibility(false);

}

void StateWifiList::TouchUpdate() {
	nexLoop(_listenList);
}

#endif
