/*
 * NextionHMI.cpp
 *
 *  Created on: 31 ��� 2018 �.
 *      Author: Azarov
 */

#include "../../../MK4duo.h"

#ifdef NEXTION_HMI

#include "NextionHMI.h"
#include "../nextion/library/Nextion.h"

#include "NextionConstants.h"

namespace {
	bool _nextionOn = false;
	uint8_t _pageID = 0;
	bool _sdInserted = false;
	uint8_t _rootState = 0;

#if HAS_SD_SUPPORT
  NexUpload Firmware(NEXTION_FIRMWARE_FILE, 57600);
#endif

}

uint16_t NextionHMI::autoPreheatTempHotend = PREHEAT_1_TEMP_HOTEND;
uint16_t NextionHMI::autoPreheatTempBed = PREHEAT_1_TEMP_BED;
uint8_t  NextionHMI::lcdBrightness = 90;

uint8_t NextionHMI::wizardData = 0;
int16_t NextionHMI::pageData = 0;

HMIevent NextionHMI::lastEvent = HMIevent::NONE;
uint8_t  NextionHMI::lastEventArg = 0;

NexObject NextionHMI::headerText = NexObject(0,  0,  "tH");
NexObject NextionHMI::headerIcon = NexObject(0,  0,  "iH");

NexObject NextionHMI::wifiStatus = NexObject(0,  0,  "wifi");
NexObject NextionHMI::ethernetStatus = NexObject(0,  0,  "eth");
NexObject NextionHMI::acStatus = NexObject(0,  0,  "ac");

NexObject NextionHMI::statusIcon0 = NexObject(0,  0,  "iS0");
NexObject NextionHMI::statusIcon1 = NexObject(0,  0,  "iS1");
NexObject NextionHMI::statusIcon2 = NexObject(0,  0,  "iS2");

NexObject* NextionHMI::statusIcons[] = {&NextionHMI::statusIcon0, &NextionHMI::statusIcon1, &NextionHMI::statusIcon2, nullptr};

char NextionHMI::buffer[NEXHMI_BUFFER_SIZE] = {0};

void NextionHMI::Init() {

	for (uint8_t i = 0; i < 10; i++) {
		ZERO(buffer);
		_nextionOn = nexInit(buffer);
		if (_nextionOn)
		{
			break;
		}
		HAL::delayMilliseconds(500);
	}

	if (!_nextionOn) {
		SERIAL_MSG("Nextion LCD serial not initialized! \n");
		return;
	}

	//Retreiving model
	if (strstr(buffer, "NX")) {
		SERIAL_MSG("Nextion LCD connected!  \n");
	}
	else
	{
		_nextionOn = false;
		SERIAL_MSG("Nextion LCD NOT connected! \n");
	}

	//Init Pages
	StateStatus::Init();
	StateFiles::Init();
	StateFileinfo::Init();
	StatePrinting::Init();
	StateMovement::Init();
	StateAbout::Init();
	StateSettings::Init();
	StateConnections::Init();
	StateWifiList::Init();
	StateAuraConnect::Init();
	StateAuraConnectStatus::Init();

	wifiStatus.setGlobalValue(0);
	ethernetStatus.setGlobalValue(0);
	acStatus.setGlobalValue(0);
}

void NextionHMI::DrawUpdate() {
	//check SD card
	#if HAS_SD_SUPPORT && PIN_EXISTS(SD_DETECT)

	Serial.println(READ(SD_DETECT_PIN));

		const bool sd_status = IS_SD_INSERTED;
		if (sd_status != _sdInserted) {
		    _sdInserted = sd_status;
		    if (sd_status) {
		    	printer.safe_delay(1000);
			    Serial.println("SD inserted!");
			  card.mount();
		    }
		    else {
			    Serial.println("SD out!");
			  card.unmount();
		    }
		    UpdateSDIcon();

		}
	  #endif // HAS_SD_SUPPORT && SD_DETECT_PIN

	switch(_pageID) {
	    case PAGE_STATUS : StateStatus::DrawUpdate();
	         break;
	    case PAGE_TEMPERATURE : StateTemperature::DrawUpdate();
	         break;
	    case PAGE_FILES : //Nothing to update
	         break;
	    case PAGE_FILEINFO : //Nothing to update
	         break;
	    case PAGE_PRINTING : StatePrinting::DrawUpdate();
	         break;
	    case PAGE_MESSAGE : //Nothing to update
	         break;
	    case PAGE_WIZARD : StateWizard::DrawUpdate();
	         break;
	    case PAGE_MENU : //Maintenance_DrawUpdate();
	         break;
	    case PAGE_MOVEMENT :StateMovement::DrawUpdate();
	         break;
	    case PAGE_SETTINGS :
	         break;
	    case PAGE_ABOUT :
	         break;
	    case PAGE_WIZARDZ : StateWizardZ::DrawUpdate();
	         break;
	    case PAGE_DINFO : StateDInfo::DrawUpdate();
	         break;
	    case PAGE_C_NUMBER :
	         break;
	    case PAGE_C_KEY :
	         break;
	    case PAGE_CONNECTIONS :
	         break;
	    case PAGE_WIFILIST :
	         break;
	    case PAGE_ACCONFIG :
	         break;
	    case PAGE_ACSTATUS :
	         break;
	}
}

void NextionHMI::TouchUpdate() {
	switch(_pageID) {
	    case PAGE_STATUS : StateStatus::TouchUpdate();
	         break;
	    case PAGE_TEMPERATURE : StateTemperature::TouchUpdate();
	         break;
	    case PAGE_FILES : StateFiles::TouchUpdate();
	         break;
	    case PAGE_FILEINFO : StateFileinfo::TouchUpdate();
	         break;
	    case PAGE_PRINTING : StatePrinting::TouchUpdate();
	         break;
	    case PAGE_MESSAGE : StateMessage::TouchUpdate();
	         break;
	    case PAGE_WIZARD : StateWizard::TouchUpdate();
	         break;
	    case PAGE_MENU : StateMenu::TouchUpdate();
	         break;
	    case PAGE_MOVEMENT : StateMovement::TouchUpdate();
	         break;
	    case PAGE_SETTINGS : StateSettings::TouchUpdate();
	         break;
	    case PAGE_ABOUT : StateAbout::TouchUpdate();
	         break;
	    case PAGE_DINFO : StateDInfo::TouchUpdate();
	         break;
	    case PAGE_WIZARDZ : StateWizardZ::TouchUpdate();
	         break;
	    case PAGE_C_NUMBER : StateEditNumber::TouchUpdate();
	         break;
	    case PAGE_C_KEY : StateKeyboard::TouchUpdate();
	         break;
	    case PAGE_CONNECTIONS : StateConnections::TouchUpdate();
	         break;
	    case PAGE_WIFILIST : StateWifiList::TouchUpdate();
	         break;
	    case PAGE_ACCONFIG : StateAuraConnect::TouchUpdate();
	         break;
	    case PAGE_ACSTATUS : StateAuraConnectStatus::TouchUpdate();
	         break;
	}
}

void NextionHMI::ActivateState(uint8_t state_id) {
	_pageID = state_id;
	if (_pageID == PAGE_STATUS) _rootState = PAGE_STATUS;
	else if (_pageID == PAGE_PRINTING) _rootState = PAGE_PRINTING;

#if HAS_SD_SUPPORT && PIN_EXISTS(SD_DETECT)
    UpdateSDIcon();
#endif
}

void NextionHMI::ShowState(uint8_t state_id) {
	switch(state_id) {
		    case PAGE_STATUS : StateStatus::Activate();
		         break;
		    case PAGE_TEMPERATURE : StateStatus::Activate();
		         break;
		    case PAGE_FILES : ShowState(_rootState);
		         break;
		    case PAGE_FILEINFO : ShowState(_rootState);
		         break;
		    case PAGE_PRINTING : StatePrinting::Activate();
		         break;
		    case PAGE_MESSAGE : ShowState(_rootState);
		         break;
		    case PAGE_WIZARD : ShowState(_rootState);
		         break;
		    case PAGE_MENU : ShowState(_rootState);
		         break;
		    case PAGE_MOVEMENT : StateMovement::Activate(MODE_MOVE_AXIS);
		         break;
		    case PAGE_SETTINGS : ShowState(_rootState);
		         break;
		    case PAGE_ABOUT : StateAbout::Activate();
		         break;
		    case PAGE_WIZARDZ : ShowState(_rootState);
		         break;
		    case PAGE_C_NUMBER : ShowState(_rootState);
		         break;
		    case PAGE_C_KEY : ShowState(_rootState);
		         break;
		    case PAGE_CONNECTIONS : StateConnections::Activate();
		         break;
		    case PAGE_WIFILIST : StateWifiList::Activate();
		         break;
		    case PAGE_ACCONFIG : StateAuraConnect::Activate();
		         break;
		    case PAGE_ACSTATUS : StateAuraConnectStatus::Activate();
		         break;
		}
}

void NextionHMI::RaiseEvent(HMIevent event, uint8_t eventArg, const char *eventMsg) {
	lastEvent = event;
	lastEventArg = eventArg;
	//Error handling and common events
	switch(event) {
		case HMIevent::PRINTER_KILLED : StateMessage::ActivatePGM(MESSAGE_CRITICAL_ERROR, NEX_ICON_ERROR, eventMsg, PSTR(MSG_ERR_KILLED), 1, PSTR(RESTART_TO_CONTINUE), 0, 0, 0, NEX_ICON_DIALOG_ERROR);
			return;
		case HMIevent::SWITCHED_OFF : StateMessage::ActivatePGM(MESSAGE_CRITICAL_ERROR, NEX_ICON_ERROR, eventMsg, PSTR(MSG_OFF), 1, PSTR(RESTART_TO_CONTINUE), 0, 0, 0, NEX_ICON_DIALOG_ERROR);
			return;
		case HMIevent::TEMPERATURE_ERROR :
			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR("%s\\r%s %d"), eventMsg, MSG_STOPPED_HEATER, eventArg);
			StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, MSG_HEATER_ERROR, NextionHMI::buffer, 2, PSTR(MSG_OK), StateMessage::ReturnToLastState, PSTR(MSG_RETRY), StateMessage::RetryHeaterAndReturnToLastState, NEX_ICON_DIALOG_ERROR);
			return;
		case HMIevent::EEPROM_ERROR :
			StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, MSG_ERROR, eventMsg, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0, NEX_ICON_DIALOG_ERROR);
			return;
		case HMIevent::SD_ERROR :
			if (eventArg!=0)
			{
				ZERO(NextionHMI::buffer);
				sprintf_P(NextionHMI::buffer, PSTR("%s %d"), eventMsg, eventArg);
				StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, NextionHMI::buffer, eventMsg, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0, NEX_ICON_DIALOG_ERROR);
			}
			else
				StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, MSG_ERROR, eventMsg, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0, NEX_ICON_DIALOG_ERROR);
			return;
		case HMIevent::ERROR :
			StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, MSG_ERROR, eventMsg, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0, NEX_ICON_DIALOG_ERROR);
			return;
		case HMIevent::WAIT_FOR_INPUT :
			StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_INFO, eventMsg, PSTR(MSG_USERWAIT), 1, PSTR(MSG_OK), WaitOk_Push, 0, 0);
			return;
	    case HMIevent::SD_PRINT_FINISHED :
	    	StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_FINISHED, PSTR(MSG_FINISHED), PSTR(MSG_DONE), 2, PSTR(MSG_OK), StatePrinting::DoneMessage_OK, PSTR(MSG_PRINT_AGAIN),StatePrinting::DoneMessage_Again, NEX_ICON_DIALOG_DONE);
	        NextionHMI::RaiseEvent(NONE);
	        return;
	    case HMIevent::HOST_PRINT_FINISHED :
	    	StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_FINISHED, PSTR(MSG_FINISHED), PSTR(MSG_DONE), 1, PSTR(MSG_OK), StatePrinting::DoneMessage_OK, nullptr, nullptr, NEX_ICON_DIALOG_DONE);
	        NextionHMI::RaiseEvent(NONE);
	        return;
	}

	switch(_pageID) {
	    case PAGE_STATUS :
	         break;
	    case PAGE_TEMPERATURE :
	         break;
	    case PAGE_FILES :
	         break;
	    case PAGE_FILEINFO :
	         break;
	    case PAGE_PRINTING : StatePrinting::OnEvent(event, eventArg);
	         break;
	    case PAGE_MESSAGE :
	         break;
	    case PAGE_WIZARD :
	         break;
	    case PAGE_MENU :
	         break;
	    case PAGE_MOVEMENT :
	         break;
	    case PAGE_SETTINGS :
	         break;
	    case PAGE_ABOUT :
	         break;
	    case PAGE_WIZARDZ :
	         break;
	    case PAGE_C_NUMBER :
	         break;
	    case PAGE_C_KEY :
	         break;
	    case PAGE_CONNECTIONS :
	         break;
	    case PAGE_WIFILIST :
	         break;
	    case PAGE_ACCONFIG :
	         break;
	    case PAGE_ACSTATUS :
	         break;
	}
}

uint8_t NextionHMI::GetActiveState() {
	return _pageID;
}

void NextionHMI::WaitOk_Push(void* ptr) {
    printer.setWaitForUser(false);
    StateMessage::ReturnToLastState(ptr);
}

void NextionHMI::UploadFirmwareFromSD(uint8_t sd_slot) {
  if (sdStorage.isSdInserted(sd_slot) && sdStorage.cards[sd_slot].isMounted()) {
    Firmware.startUpload(sd_slot);
    nexSerial.end();
    Init();
	SetBrightness(lcdBrightness);
  }
}

void NextionHMI::UploadFirmwareFromSerial(uint32_t tftSize, uint8_t serialPort) {
    Firmware.uploadFromSerial(tftSize, serialPort);
    SERIAL_PORT(-1);
    nexSerial.end();
    Init();
	SetBrightness(lcdBrightness);
}

void NextionHMI::ShowStartScreen(const char* header, const char* message) {
	NexObject startScreen = NexObject(0,  0,  "start");
	NexObject startHeader = NexObject(0,  2,  "t1");
	NexObject startMessage = NexObject(0,  3,  "t2");
	startScreen.show();
	startHeader.setText(header);
	startMessage.setText(message);
}

void NextionHMI::SetBrightness(uint8_t brightness) {
	setCurrentBrightness(brightness);
}


bool _wifiEnabled;
bool _wifiConnected;
bool _ethernetConnected;
bool _acConnected;

void NextionHMI::SetNetworkStatus(bool wifiEnabled, bool wifiConnected, bool ethernetConnected, bool acConnected) {

	wifiStatus.setGlobalValue(wifiEnabled ? (wifiConnected ? NEX_WIFI_CONNECTED : NEX_WIFI_DISCONNECTED) : 0);
	ethernetStatus.setGlobalValue(ethernetConnected ? 1 : 0);
	acStatus.setGlobalValue(acConnected ? 1 : 0);
	uint8_t statusIconsCount = 0;

	if (wifiEnabled)
	{
		if (wifiConnected)
		{
			statusIcons[statusIconsCount]->setPic(NEX_ICON_WIFI_CONNECTED);
		}
		else
		{
			statusIcons[statusIconsCount]->setPic(NEX_ICON_WIFI_DISCONNECTED);
		}
		statusIconsCount++;
	}

	if (ethernetConnected)
	{
		statusIcons[statusIconsCount]->setPic(NEX_ICON_ETH_CONNECTED);
		statusIconsCount++;
	}

	if (acConnected)
	{
		statusIcons[statusIconsCount]->setPic(NEX_ICON_AC_CONNECTED);
		statusIconsCount++;
	}

	for (int i = statusIconsCount; statusIcons[i] != nullptr; i++)
	{
		statusIcons[i]->setPic(NEX_ICON_BLANK_HEADER);
	}

}

#if HAS_SD_SUPPORT && PIN_EXISTS(SD_DETECT)
void NextionHMI::UpdateSDIcon() {
	if (_sdInserted)
	{
		sdIcon.setPic(NEX_ICON_SD);
		sdText.setTextPGM(PSTR("SD"));
	}
	else
	{
		sdIcon.setPic(NEX_ICON_NO_SD);
		sdText.setTextPGM(PSTR(""));
	}
}
#endif

#endif

