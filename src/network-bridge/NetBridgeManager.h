/*
 * NetBridgeManager.h
 *
 *  Created on: 9.10.2020.
 *      Author: Avazar
 */

#pragma once

enum NetBridgeStatus {
	NotConnected,
	Connected,
	TempDisconnected
};

class NetBridgeManager {
private:
    NetBridgeStatus _netBridgeStatus;

    bool _wifiEnabled;
    bool _wifiConnected;
    bool _ethernetConnected;
    bool _acConnected;

    bool _jobAwaiting;

#if ENABLED(NEXTION_HMI)
    void UpdateNextionStatusIcons();
#endif

public:
	NetBridgeManager();

	bool CheckBridgeSerialConnection();
	bool ConnectWifi(const char* ssid, const char* password, char* responseBuffer, const uint16_t responseBufferSize);
	bool SwitchWifi(bool enabled, char* responseBuffer, const uint16_t responseBufferSize);
	bool GetWifiNetworks(char* responseBuffer, const uint16_t responseBufferSize);
	bool GetWifiConnected(bool &connected, char* responseBuffer, const uint16_t responseBufferSize);
	bool GetEthernetConnected(bool &connected, char* responseBuffer, const uint16_t responseBufferSize);
	bool AcBridgeVersion(char* responseBuffer, const uint16_t responseBufferSize);
	bool AcBridgeInfo(char* responseBuffer, const uint16_t responseBufferSize);
	bool SetAcServerUri(const char* uri, char* responseBuffer, const uint16_t responseBufferSize);
	bool SetAcServerId(const char* id, char* responseBuffer, const uint16_t responseBufferSize);
	bool GetAcServerUri(char* responseBuffer, const uint16_t responseBufferSize);
	bool GetAcServerId(char* responseBuffer, const uint16_t responseBufferSize);
	bool ConnectAcServer(const char* uri, const char* id, const char* code, char* responseBuffer, const uint16_t responseBufferSize);
	bool SetSdSlotIndex(uint8_t index, char* responseBuffer, const uint16_t responseBufferSize);
	bool GetSdSlotIndex(uint8_t &index, char* responseBuffer, const uint16_t responseBufferSize);
	bool RebootBridge();

	bool SendCommand(const char* command, char* responseBuffer, uint16_t responseBufferSize);

	NetBridgeStatus GetNetBridgeStatus();
	void UpdateNetBridgeStatus(NetBridgeStatus status);

	bool IsWifiEnabled();
	bool IsWifiConnected();
	bool IsEthernetConnected();
	bool IsAcConnected();

	void UpdateWifiEnabled(bool wifiEnabled);
	void UpdateWifiConnected(bool wifiConnected);
	void UpdateEthernetConnected(bool ethernetConnected);
	void UpdateAcConnected(bool acConnected);

	bool IsJobAwaiting();
	void UpdateJobAwaiting(bool jobAwaiting);


};

extern NetBridgeManager netBridgeManager;

