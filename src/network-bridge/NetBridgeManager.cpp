/*
 * NetBridgeManager.cpp
 *
 *  Created on: 9.10.2020.
 *      Author: Avazar
 */

#include "../../MK4duo.h"

NetBridgeManager netBridgeManager;

uint16_t NetBridgeManager::SendCommand(const char *command,
		char *responseBuffer, uint16_t responseBufferSize) {

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
      if (serial_char == '\n' || serial_char == '\r' || responseSize == responseBufferSize) {
    	  return responseBufferSize;
      }
      else
      {
    	  responseBuffer[responseSize] = c;
      }
  }
  return 0;

}

NetBridgeManager::NetBridgeManager() {

}

bool NetBridgeManager::CheckBridgeSerialConnection() {
	char buffer[8];
	auto count = SendCommand("@ping", buffer, sizeof(buffer));
	if (count == 2 && buffer[0] == 'o' && buffer[0] == 'k') return true;
	return false;
}

bool NetBridgeManager::ConnectWifi(const char *ssid, const char *password,
		char *responseBuffer, const uint16_t responseBufferSize,
		uint16_t &responseSize) {
	char commandBuffer[128];
	snprintf()

	auto command = String("@wifi_connect " + ssid + " " + password) ;


}

bool NetBridgeManager::SwitchWifi(bool enabled, char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::GetWifiNetworks(char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::IsWifiConnected(bool &connected, char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::IsEthernetConnected(bool &connected,
		char *responseBuffer, const uint16_t responseBufferSize,
		uint16_t &responseSize) {
}

bool NetBridgeManager::AcBridgeVersion(char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::AcBridgeInfo(char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::SetAcServerUri(const char *uri, char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::SetAcServerId(const char *id, char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::GetAcServerUri(char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::GetAcServerId(char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::ConnectAcServer(const char *uri, const char *id,
		const char *code, char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::SetSdSlotIndex(uint8_t index, char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::GetSdSlotIndex(uint8_t &index, char *responseBuffer,
		const uint16_t responseBufferSize, uint16_t &responseSize) {
}

bool NetBridgeManager::RebootBridge() {
}
