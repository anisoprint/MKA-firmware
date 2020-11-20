/*
 * NetBridgeManager.cpp
 *
 *  Created on: 9.10.2020.
 *      Author: Avazar
 */

#include "../../MK4duo.h"

NetBridgeManager netBridgeManager;

bool NetBridgeManager::SendCommand(const char *command,
		char *responseBuffer, uint16_t responseBufferSize) {

  int8_t oldSerial = Com::serial_port_index;
  SERIAL_PORT(NETWORK_BRIDGE_SERIAL);
  SERIAL_FLUSH();
  SERIAL_ET(command);

  uint16_t responseSize = 0;

  watch_t responseWatch = watch_t(NETWORK_BRIDGE_TIMEOUT);
  while (!responseWatch.elapsed())
  {
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
}

bool NetBridgeManager::CheckBridgeSerialConnection() {
	char buffer[8];
	bool res = SendCommand("@ping", buffer, sizeof(buffer));
	bool connected = (res && strncmp(buffer, "ok", 2)==0);
	_netBridgeStatus = Connected;
  return connected;
}

bool NetBridgeManager::ConnectWifi(const char *ssid, const char *password,
		char *responseBuffer, const uint16_t responseBufferSize) {

	char commandBuffer[128];
	snprintf(commandBuffer, sizeof(commandBuffer), "@wifi_connect %s %s", ssid, password);
	bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize);

  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::SwitchWifi(bool enabled, char *responseBuffer,
		const uint16_t responseBufferSize) {
  char commandBuffer[8];
  snprintf(commandBuffer, sizeof(commandBuffer), "@wifi %u", enabled);
  bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize);

  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::GetWifiNetworks(char *responseBuffer,
		const uint16_t responseBufferSize) {

  bool res = SendCommand("@wifi_list", responseBuffer, responseBufferSize);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);

}

bool NetBridgeManager::GetWifiConnected(bool &connected, char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@wifi_status", responseBuffer, responseBufferSize);

  connected = responseBuffer[0] == '1';
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::GetEthernetConnected(bool &connected,
		char *responseBuffer, const uint16_t responseBufferSize) {
  bool res = SendCommand("@ethernet_status", responseBuffer, responseBufferSize);

  connected = responseBuffer[0] == '1';
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::AcBridgeVersion(char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@ver", responseBuffer, responseBufferSize);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::AcBridgeInfo(char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@info", responseBuffer, responseBufferSize);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::SetAcServerUri(const char *uri, char *responseBuffer,
		const uint16_t responseBufferSize) {
  char commandBuffer[128];
  snprintf(commandBuffer, sizeof(commandBuffer), "@ac_server_uri %s", uri);
  bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize);

  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::SetAcServerId(const char *id, char *responseBuffer,
		const uint16_t responseBufferSize) {
  char commandBuffer[128];
  snprintf(commandBuffer, sizeof(commandBuffer), "@ac_server_id %s", id);
  bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize);

  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::GetAcServerUri(char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@ac_server_uri", responseBuffer, responseBufferSize);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::GetAcServerId(char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@ac_server_id", responseBuffer, responseBufferSize);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::ConnectAcServer(const char *uri, const char *id,
		const char *code, char *responseBuffer,
		const uint16_t responseBufferSize) {
  char commandBuffer[256];
  snprintf(commandBuffer, sizeof(commandBuffer), "@ac_server_connect %s %s %s", uri, id, code);
  bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize);

  return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::SetSdSlotIndex(uint8_t index, char *responseBuffer,
		const uint16_t responseBufferSize) {
    char commandBuffer[20];
    snprintf(commandBuffer, sizeof(commandBuffer), "@sd_slot_index %u", index);
    bool res = SendCommand(commandBuffer, responseBuffer, responseBufferSize);

    return (res && strncmp(responseBuffer, "ok", 2)==0);
}

bool NetBridgeManager::GetSdSlotIndex(uint8_t &index, char *responseBuffer,
		const uint16_t responseBufferSize) {
  bool res = SendCommand("@ac_server_id", responseBuffer, responseBufferSize);
  index = atoi(responseBuffer);
  return (res && strncmp(responseBuffer, "Error", 5)!=0);
}

bool NetBridgeManager::RebootBridge() {
  char responseBuffer[8];
  bool res = SendCommand("@reboot", responseBuffer, sizeof(responseBuffer));
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
	#if ENABLED(NEXTION_HMI)
		UpdateNextionStatusIcons();
	#endif
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

#endif
