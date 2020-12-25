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

struct WifiNetworkInfo
{
	bool connected;
	bool secure;
	uint8_t signal;
	char ssid[32];
};

class NetBridgeManager {
private:
    NetBridgeStatus _netBridgeStatus;

    bool _wifiEnabled;
    bool _wifiConnected;
    bool _ethernetConnected;
    bool _acConnected;
    bool _tcpEnabled;

    bool _jobAwaiting;

    char _selectedWifiSsid[32];

#if ENABLED(NEXTION_HMI)
    void UpdateNextionStatusIcons();
#endif

public:
    static const uint8_t wifiListSize = 7;
    WifiNetworkInfo wifiNetworks[wifiListSize];
    uint8_t startNetworkIndex;
    uint8_t networksCount;

	NetBridgeManager();

	void SyncWithNetworkBridge();

	bool CheckBridgeSerialConnection();
	bool ConnectWifi(const char* ssid, const char* password, char* responseBuffer, const uint16_t responseBufferSize);
	bool SwitchWifi(bool enabled, char* responseBuffer, const uint16_t responseBufferSize);
	bool ScanWifiNetworks();
	bool GetWifiNetworks(char* responseBuffer, const uint16_t responseBufferSize, const uint8_t beginIndex, const uint8_t networksNumber);
	bool GetWifiConnected(bool &connected, char* responseBuffer, const uint16_t responseBufferSize);
	bool GetEthernetConnected(bool &connected, char* responseBuffer, const uint16_t responseBufferSize);
	bool GetNetBridgeVersion(char* responseBuffer, const uint16_t responseBufferSize);
	bool GetNetBridgeInfo(char* responseBuffer, const uint16_t responseBufferSize);
	bool SetAcServerUri(const char* uri, char* responseBuffer, const uint16_t responseBufferSize);
	bool SetAcServerId(const char* id, char* responseBuffer, const uint16_t responseBufferSize);
	bool GetAcServerUrl(char* responseBuffer, const uint16_t responseBufferSize);
	bool GetAcServerId(char* responseBuffer, const uint16_t responseBufferSize);
	bool ConnectAcServer(const char* uri, const char* code, char* responseBuffer, const uint16_t responseBufferSize);
	bool SetSdSlotIndex(uint8_t index, char* responseBuffer, const uint16_t responseBufferSize);
	bool GetSdSlotIndex(uint8_t &index, char* responseBuffer, const uint16_t responseBufferSize);
	bool GetTcpEnabled(bool &enabled, char* responseBuffer, const uint16_t responseBufferSize);
	bool SetTcpEnabled(bool enabled, char* responseBuffer, const uint16_t responseBufferSize);

	bool GetWlanMac(char* responseBuffer, const uint16_t responseBufferSize);
	bool GetEthMac(char* responseBuffer, const uint16_t responseBufferSize);
	bool GetWlanIp4(char* responseBuffer, const uint16_t responseBufferSize);
	bool GetEthIp4(char* responseBuffer, const uint16_t responseBufferSize);

	bool RebootBridge();

	bool ListWifiNetworks(uint8_t startIndex);

	bool SendCommand(const char* command, char* responseBuffer, uint16_t responseBufferSize, millis_l timeout);

	NetBridgeStatus GetNetBridgeStatus();
	void UpdateNetBridgeStatus(NetBridgeStatus status);

	bool IsWifiEnabled();
	bool IsWifiConnected();
	bool IsEthernetConnected();
	bool IsAcConnected();

	void SelectWifiNetwork(const char* ssid);

	//TODO: For backward compatibility, remove in a while
	void UpdateConnectedNetwork();
	char* GetSelectedWifiNetwork();

	bool IsTcpEnabled();

	void UpdateWifiEnabled(bool wifiEnabled);
	void UpdateWifiConnected(bool wifiConnected);
	void UpdateEthernetConnected(bool ethernetConnected);
	void UpdateAcConnected(bool acConnected);

	bool IsJobAwaiting();
	void UpdateJobAwaiting(bool jobAwaiting);


};

extern NetBridgeManager netBridgeManager;

