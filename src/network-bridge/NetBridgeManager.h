/*
 * NetBridgeManager.h
 *
 *  Created on: 9.10.2020.
 *      Author: Avazar
 */

#pragma once

class NetBridgeManager {
private:
	uint16_t SendCommand(const char* command, char* responseBuffer, uint16_t responseBufferSize);
public:
	NetBridgeManager();

	bool CheckBridgeSerialConnection();
	bool ConnectWifi(const char* ssid, const char* password, char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool SwitchWifi(bool enabled, char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool GetWifiNetworks(char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool IsWifiConnected(bool &connected, char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool IsEthernetConnected(bool &connected, char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool AcBridgeVersion(char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool AcBridgeInfo(char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool SetAcServerUri(const char* uri, char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool SetAcServerId(const char* id, char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool GetAcServerUri(char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool GetAcServerId(char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool ConnectAcServer(const char* uri, const char* id, const char* code, char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool SetSdSlotIndex(uint8_t index, char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool GetSdSlotIndex(uint8_t &index, char* responseBuffer, const uint16_t responseBufferSize, uint16_t &responseSize);
	bool RebootBridge();
};

extern NetBridgeManager netBridgeManager;

