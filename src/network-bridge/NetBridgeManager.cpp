/*
 * NetBridgeManager.cpp
 *
 *  Created on: 9.10.2020.
 *      Author: Avazar
 */

#include "../../MK4duo.h"
#include <ArduinoJson.h>

NetBridgeManager netBridgeManager;

bool NetBridgeManager::SendCommand(const char *command,
		char *responseBuffer, uint16_t responseBufferSize, millis_l timeout) {

  int8_t oldSerial = Com::serial_port_index;
  SERIAL_PORT(NETWORK_BRIDGE_SERIAL);
  SERIAL_FLUSH();
  SERIAL_ET(command);

  uint16_t responseSize = 0;

  watch_t responseWatch = watch_t(timeout);
  while (!responseWatch.elapsed())
  {
      printer.check_periodical_actions();
      const int c = Com::serialRead(NETWORK_BRIDGE_SERIAL);
      if (c < 0) continue;
      const char serial_char = c;
      if (serial_char == '\n' || serial_char == '\r' || responseSize == responseBufferSize - 1) {
        responseBuffer[responseSize] = '\0';
        SERIAL_PORT(oldSerial);
    	return true;
      }
      else
      {
    	  responseBuffer[responseSize] = c;
    	  responseSize++;
      }
  }
  SERIAL_PORT(oldSerial);
  return false;

}

NetBridgeManager::NetBridgeManager() {
  _netBridgeStatus = NotConnected;
  _wifiEnabled = false;
  _wifiConnected = false;
  _ethernetConnected = false;
  _acConnected = false;
  _jobAwaiting = false;
  _tcpEnabled = false;
  startNetworkIndex = 0;
  networksCount = 0;
}

bool NetBridgeManager::CheckBridgeSerialConnection() {
	char buffer[8];
	bool res = SendCommand("@ping", buffer, sizeof(buffer), NETWORK_BRIDGE_TIMEOUT);
	bool connected = (res && strncmp(buffer, "ok", 2)==0);
	_netBridgeStatus = Connected;
  return connected;
}

bool NetBridgeManager::ConnectWifi(const char *ssid, const char *password,
		char *responseBuffer, const uint16_t responseBufferSize) {

	char commandBuffer[128];
	snprintf(commandBuffer, sizeof(commandBuffer), "@wifi_connect %s %s", ssid, password);
	bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize, 25000);

  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::SwitchWifi(bool enabled, char *responseBuffer,
		const uint16_t responseBufferSize) {
  char commandBuffer[8];
  snprintf(commandBuffer, sizeof(commandBuffer), "@wifi %u", enabled);
  bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);

  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::GetWifiNetworks(char *responseBuffer,
		const uint16_t responseBufferSize, const uint8_t beginIndex, const uint8_t networksNumber) {
  char commandBuffer[16];
  snprintf(commandBuffer, sizeof(commandBuffer), "@wifi_list %u %u", beginIndex, networksNumber);
  bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);

}

bool NetBridgeManager::ScanWifiNetworks() {
  char responseBuffer[8];
  bool res = SendCommand("@wifi_scan", responseBuffer, sizeof(responseBuffer), NETWORK_BRIDGE_TIMEOUT);
  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::GetWifiConnected(bool &connected, char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@wifi_status", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);

  connected = responseBuffer[0] == '1';
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::GetEthernetConnected(bool &connected,
		char *responseBuffer, const uint16_t responseBufferSize) {
  bool res = SendCommand("@ethernet_status", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);

  connected = responseBuffer[0] == '1';
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::GetNetBridgeVersion(char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@ver", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::GetNetBridgeInfo(char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@info", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::SetAcServerUri(const char *uri, char *responseBuffer,
		const uint16_t responseBufferSize) {
  char commandBuffer[128];
  snprintf(commandBuffer, sizeof(commandBuffer), "@ac_server_uri %s", uri);
  bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);

  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::SetAcServerId(const char *id, char *responseBuffer,
		const uint16_t responseBufferSize) {
  char commandBuffer[128];
  snprintf(commandBuffer, sizeof(commandBuffer), "@ac_server_id %s", id);
  bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);

  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::GetAcServerUrl(char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@ac_server_uri", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::GetAcServerId(char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@ac_server_id", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::ConnectAcServer(const char *uri, const char *code, char *responseBuffer,
		const uint16_t responseBufferSize) {
  char commandBuffer[256];
  snprintf(commandBuffer, sizeof(commandBuffer), "@ac_server_connect %s %s", uri, code);
  bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT*2);

  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::SetSdSlotIndex(uint8_t index, char *responseBuffer,
		const uint16_t responseBufferSize) {
    char commandBuffer[20];
    snprintf(commandBuffer, sizeof(commandBuffer), "@sd_slot_index %u", index);
    bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);

    return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::GetSdSlotIndex(uint8_t &index, char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@sd_slot_index", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
  index = atoi(responseBuffer);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::RebootBridge() {
  char responseBuffer[8];
  bool res = SendCommand("@reboot", responseBuffer, sizeof(responseBuffer), NETWORK_BRIDGE_TIMEOUT);
  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

NetBridgeStatus NetBridgeManager::GetNetBridgeStatus() {
	return _netBridgeStatus;
}

void NetBridgeManager::UpdateNetBridgeStatus(NetBridgeStatus status) {
	_netBridgeStatus = status;
	if (status != Connected)
	{
		_wifiEnabled = false;
		_wifiConnected = false;
		_ethernetConnected = false;
		_acConnected = false;
		_jobAwaiting = false;
		_tcpEnabled = false;
	#if ENABLED(NEXTION_HMI)
		UpdateNextionStatusIcons();
	#endif
	}
	else
	{
		SyncWithNetworkBridge();
	}


}

bool NetBridgeManager::IsWifiEnabled() {
	return _wifiEnabled;
}

bool NetBridgeManager::IsWifiConnected() {
	return _wifiConnected;
}

bool NetBridgeManager::IsEthernetConnected() {
	return _ethernetConnected;
}

bool NetBridgeManager::IsAcConnected() {
	return _acConnected;
}

void NetBridgeManager::UpdateWifiEnabled(bool wifiEnabled) {
	_wifiEnabled = wifiEnabled;
#if ENABLED(NEXTION_HMI)
	UpdateNextionStatusIcons();
#endif
}

void NetBridgeManager::UpdateWifiConnected(bool wifiConnected) {
	_wifiConnected = wifiConnected;
#if ENABLED(NEXTION_HMI)
	UpdateNextionStatusIcons();
#endif
}

void NetBridgeManager::UpdateEthernetConnected(bool ethernetConnected) {
	_ethernetConnected = ethernetConnected;
#if ENABLED(NEXTION_HMI)
	UpdateNextionStatusIcons();
#endif
}

void NetBridgeManager::UpdateAcConnected(bool acConnected) {
	_acConnected = acConnected;
	if (!_acConnected)
	{
		UpdateJobAwaiting(false);
	}
#if ENABLED(NEXTION_HMI)
	UpdateNextionStatusIcons();
#endif
}

#if ENABLED(NEXTION_HMI)

void NetBridgeManager::UpdateNextionStatusIcons() {
	NextionHMI::SetNetworkStatus(_wifiEnabled, _wifiConnected, _ethernetConnected, _acConnected);
}

bool NetBridgeManager::IsJobAwaiting() {
	return _jobAwaiting;
}

void NetBridgeManager::UpdateJobAwaiting(bool jobAwaiting) {
	_jobAwaiting = jobAwaiting;
}

bool NetBridgeManager::GetTcpEnabled(bool &enabled, char *responseBuffer,
		const uint16_t responseBufferSize) {
	  bool res = SendCommand("@tcp_enabled", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);

	  enabled = responseBuffer[0] == '1';
	  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::SetTcpEnabled(bool enabled, char *responseBuffer,
		const uint16_t responseBufferSize) {
    char commandBuffer[20];
    snprintf(commandBuffer, sizeof(commandBuffer), "@tcp_enabled %u", enabled ? 1 : 0);
    bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
    bool ok = (res && strncmp(responseBuffer, "ok", 2)==0);
    if (ok)
    {
    	_tcpEnabled = enabled;
    }
    return (ok);
}

bool NetBridgeManager::GetWlanMac(char *responseBuffer,
		const uint16_t responseBufferSize) {
	  bool res = SendCommand("@wifi_mac", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
	  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::GetEthMac(char *responseBuffer,
		const uint16_t responseBufferSize) {
	  bool res = SendCommand("@eth_mac", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
	  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::GetWlanIp4(char *responseBuffer,
		const uint16_t responseBufferSize) {
	  bool res = SendCommand("@wifi_ip4", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
	  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::GetEthIp4(char *responseBuffer,
		const uint16_t responseBufferSize) {
	  bool res = SendCommand("@eth_ip4", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
	  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

void NetBridgeManager::SyncWithNetworkBridge() {
	char buffer[64] = {0};
	GetTcpEnabled(_tcpEnabled, buffer, 32);
	ZERO(buffer);
	SetSdSlotIndex(INTERNAL_SD_STORAGE_INDEX, buffer, 64);
}

bool NetBridgeManager::IsTcpEnabled() {
	return _tcpEnabled;
}

bool NetBridgeManager::ListWifiNetworks(uint8_t startIndex) {
    char responseBuffer[700];
    ZERO(responseBuffer);

    bool success = GetWifiNetworks(responseBuffer, sizeof(responseBuffer), startIndex, wifiListSize);
    if (!success) return false;

	const size_t capacity = 768;
	StaticJsonDocument<capacity> jsonDoc;
	DeserializationError err = deserializeJson(jsonDoc, responseBuffer, sizeof(responseBuffer));
	if (err!=DeserializationError::Ok)
	{
		return false;
	}

    startNetworkIndex = startIndex;
    networksCount = jsonDoc["Count"] | 0;

    JsonArray networks = jsonDoc["Networks"];
    for (int i = 0; i < networksCount; i++)
    {
    	JsonObject network = networks[i];
    	wifiNetworks[i].connected = network["con"];
    	wifiNetworks[i].secure = network["sec"];
    	wifiNetworks[i].signal = network["sig"];
    	strncpy(wifiNetworks[i].ssid, (const char*)(network["id"]), sizeof(wifiNetworks[i].ssid));
    }
    return true;

}

void NetBridgeManager::SelectWifiNetwork(const char *ssid) {
	ZERO(_selectedWifiSsid);
	strncpy(_selectedWifiSsid, ssid, sizeof(_selectedWifiSsid));
}

char* NetBridgeManager::GetSelectedWifiNetwork() {
	return _selectedWifiSsid;
}

void NetBridgeManager::UpdateConnectedNetwork() {
    for (int i = 0; i < networksCount; i++)
    {
    	wifiNetworks[i].connected = (strcmp(wifiNetworks[i].ssid,_selectedWifiSsid)==0) ? true : false;
    }
}

bool NetBridgeManager::GetJobInfo() {
    char responseBuffer[700];
    ZERO(responseBuffer);

    bool success = SendCommand("@get_job_info", responseBuffer, sizeof(responseBuffer), NETWORK_BRIDGE_TIMEOUT);
    if (!success) return false;

	const size_t capacity = 7*JSON_OBJECT_SIZE(2) + 240;
	StaticJsonDocument<capacity> jsonDoc;
	DeserializationError err = deserializeJson(jsonDoc, responseBuffer, sizeof(responseBuffer));
	if (err!=DeserializationError::Ok)
	{
		return false;
	}

	strncpy(serverJobName, (const char*)(jsonDoc["name"]), sizeof(serverJobName));

	serverJobInfo.PrintDuration = jsonDoc["time"] | 0;

	JsonObject extruder0 = jsonDoc["extruders"]["0"];
	if (!extruder0.isNull())
    {
    	strncpy(serverJobInfo.ExtruderInfo[0].PlasticMaterialName, (const char*)(extruder0["p"]["name"]), sizeof(serverJobInfo.ExtruderInfo[0].PlasticMaterialName));
    	serverJobInfo.ExtruderInfo[0].PlasticConsumption = extruder0["p"]["cons"];
    	strncpy(serverJobInfo.ExtruderInfo[0].FiberMaterialName, (const char*)(extruder0["f"]["name"]), sizeof(serverJobInfo.ExtruderInfo[0].FiberMaterialName));
    	serverJobInfo.ExtruderInfo[0].FiberConsumption = extruder0["f"]["cons"];
    }

	#if HOTENDS>1

	JsonObject extruder1 = jsonDoc["extruders"]["1"];
	if (!extruder1.isNull())
    {
    	strncpy(serverJobInfo.ExtruderInfo[1].PlasticMaterialName, (const char*)(extruder1["p"]["name"]), sizeof(serverJobInfo.ExtruderInfo[1].PlasticMaterialName));
    	serverJobInfo.ExtruderInfo[1].PlasticConsumption = extruder1["p"]["cons"];
    	strncpy(serverJobInfo.ExtruderInfo[1].FiberMaterialName, (const char*)(extruder1["f"]["name"]), sizeof(serverJobInfo.ExtruderInfo[1].FiberMaterialName));
    	serverJobInfo.ExtruderInfo[1].FiberConsumption = extruder1["f"]["cons"];
    }

	#endif
    return true;
}

bool NetBridgeManager::SendJobInvoke(bool accept, char *responseBuffer,
		const uint16_t responseBufferSize) {
    char commandBuffer[20];
    snprintf(commandBuffer, sizeof(commandBuffer), "@job_invoke %u", accept ? 1 : 2);
    bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
    bool ok = (res && strncmp(responseBuffer, "ok", 2)==0);
    return (ok);
}

bool NetBridgeManager::GetTcpPort(char *responseBuffer,
		const uint16_t responseBufferSize) {
	bool res = SendCommand("@tcp_port", responseBuffer, responseBufferSize, NETWORK_BRIDGE_TIMEOUT);
	bool ok = (res && strncmp(responseBuffer, "ok", 2)==0);
	return (ok);
}

bool NetBridgeManager::SendReconnect() {
	char buffer[64] = {0};
    SendCommand("@serial_reconnect", buffer, sizeof(buffer), 0);
    return true;

}

#endif
