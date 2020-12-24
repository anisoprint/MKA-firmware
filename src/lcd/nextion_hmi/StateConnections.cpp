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

	void DispalyWifiSignal(uint8_t signalLevel) {
		ZERO(NextionHMI::buffer);
		if (signalLevel < 33)
		{
			sprintf_P(NextionHMI::buffer, PSTR(MSG_SIGNAL_LOW), signalLevel);
		}
		else if (signalLevel < 66)
		{
			sprintf_P(NextionHMI::buffer, PSTR(MSG_SIGNAL_NORMAL), signalLevel);
		}
		else
		{
			sprintf_P(NextionHMI::buffer, PSTR(MSG_SIGNAL_HIGH), signalLevel);
		}
		_tWifiSignal.setText(NextionHMI::buffer);
	}
};

void StateConnections::Init() {
	_bBack.attachPush(Back_Push, &_bBack);
	_bChooseWifi.attachPush(Wifi_Push, &_bChooseWifi);

}


void StateConnections::Activate() {
	NextionHMI::ActivateState(PAGE_CONNECTIONS);
	_page.show();
	netBridgeManager.ListWifiNetworks(0);

	if (netBridgeManager.IsWifiEnabled())
	{
		if (netBridgeManager.IsWifiConnected())
		{
			bool wifiConnected = false;
		    for (int i = 0; i < netBridgeManager.networksCount; i++)
		    {
		    	if (netBridgeManager.wifiNetworks[i].connected)
		    	{
		    		wifiConnected = true;
		    		_tWifi.setText(netBridgeManager.wifiNetworks[i].ssid);
		    		DispalyWifiSignal(netBridgeManager.wifiNetworks[i].signal);
		    		if (netBridgeManager.wifiNetworks[i].secure)
		    		{
		    			_pWifi.setPic(NEX_ICON_WIFI_SEC_L);
		    		}

		    		char databuffer[32] = {0};
		    		if (netBridgeManager.GetWlanIp4(databuffer, sizeof(databuffer)))
		    		{
		    			ZERO(NextionHMI::buffer);
		    			sprintf_P(NextionHMI::buffer, PSTR(MSG_IPADDR), databuffer);
		    			_tWifiIp4.setText(NextionHMI::buffer);
		    		}
	    			ZERO(databuffer);
		    		if (netBridgeManager.GetWlanMac(databuffer, sizeof(databuffer)))
		    		{
		    			ZERO(NextionHMI::buffer);
		    			sprintf_P(NextionHMI::buffer, PSTR(MSG_MACADDR), databuffer);
		    			_tWifiMac.setText(NextionHMI::buffer);
		    		}

		    		break;
		    	}
		    }
		    if (!wifiConnected) _tWifi.setTextPGM(MSG_NO_CONNECTION);
		}
		else
		{
		    _tWifi.setTextPGM(MSG_NO_CONNECTION);
		}

	}
	else
	{
	    _tWifi.setTextPGM(MSG_WIFI_DISABLED);
	}

	if (netBridgeManager.IsEthernetConnected())
	{
		char databuffer[32] = {0};
		if (netBridgeManager.GetEthIp4(databuffer, sizeof(databuffer)))
		{
			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR(MSG_IPADDR), databuffer);
			_tEthIp4.setText(NextionHMI::buffer);
		}
		else
		{
			_tEthIp4.setTextPGM(PSTR(MSG_CONNECTED));
		}
		ZERO(databuffer);
		if (netBridgeManager.GetEthMac(databuffer, sizeof(databuffer)))
		{
			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR(MSG_MACADDR), databuffer);
			_tEthMac.setText(NextionHMI::buffer);
		}

	}
	else
	{
		_tEthIp4.setTextPGM(MSG_NO_CONNECTION);
	}


}

void StateConnections::TouchUpdate() {
	nexLoop(_listenList);
}

void StateConnections::Back_Push(void *ptr) {
	StateMenu::ActivateNetwork();
}

void StateConnections::Wifi_Push(void *ptr) {
	StateWifiList::Activate();
}


#endif

