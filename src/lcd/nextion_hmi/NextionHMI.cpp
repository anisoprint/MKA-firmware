/*
 * NextionHMI.cpp
 *
 *  Created on: 31 мая 2018 г.
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

#if HAS_SDSUPPORT
  NexUpload Firmware(NEXTION_FIRMWARE_FILE, 57600);
#endif

}

uint16_t NextionHMI::autoPreheatTempHotend = PREHEAT_1_TEMP_HOTEND;
uint16_t NextionHMI::autoPreheatTempBed = PREHEAT_1_TEMP_BED;
uint8_t  NextionHMI::lcdBrightness = 90;

uint8_t NextionHMI::wizardData = 0;
uint8_t NextionHMI::pageData = 0;

HMIevent NextionHMI::lastEvent = HMIevent::NONE;
uint8_t  NextionHMI::lastEventArg = 0;

NexObject NextionHMI::headerText = NexObject(0,  0,  "tH");
NexObject NextionHMI::headerIcon = NexObject(0,  0,  "iH");
NexObject NextionHMI::sdText = NexObject(0,  0,  "tSD");
NexObject NextionHMI::sdIcon = NexObject(0,  0,  "iSD");

char NextionHMI::buffer[256] = {0};

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

	  //serial_print("\n>>>>>>>>>>>>\n");
	  //serial_print(buffer);
	  //serial_print("\n>>>>>>>>>>>>\n");

	if (strstr(buffer, "NX4832T035")) {
		SERIAL_MSG("Nextion LCD connected!  \n");
	}
	else
	{
		_nextionOn = false;
		SERIAL_MSG("Nextion LCD NOT connected! \n");
	}

	SetBrightness(lcdBrightness);

	/*
	//Init Pages
	Status_Init();
	Temperature_Init();
	Maintenance_Init();
	Files_Init();
	Fileinfo_Init();
	Printing_Init();
	Message_Init();
	Movement_Init();



	Status_Activate();*/

	StateStatus::Init();
	StateFiles::Init();
	StateFileinfo::Init();
	StatePrinting::Init();
	StateMovement::Init();
	StateWizardZ::Init();
	StateAbout::Init();
	StateSettings::Init();


}

void NextionHMI::DrawUpdate() {
	//check SD card
	#if HAS_SDSUPPORT && PIN_EXISTS(SD_DETECT)

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
	  #endif // HAS_SDSUPPORT && SD_DETECT_PIN

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
	    case PAGE_C_NUMBER :
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
	    case PAGE_WIZARDZ : StateWizardZ::TouchUpdate();
	         break;
	    case PAGE_C_NUMBER : StateEditNumber::TouchUpdate();
	         break;
	}
}

void NextionHMI::ActivateState(uint8_t state_id) {
	_pageID = state_id;
#if HAS_SDSUPPORT && PIN_EXISTS(SD_DETECT)
    UpdateSDIcon();
#endif
}

void NextionHMI::ShowState(uint8_t state_id) {
	switch(state_id) {
		    case PAGE_STATUS : StateStatus::Activate();
		         break;
		    case PAGE_TEMPERATURE : StateStatus::Activate();
		         break;
		    case PAGE_FILES : StateFiles::Activate();
		         break;
		    case PAGE_FILEINFO : StateFileinfo::Activate();
		         break;
		    case PAGE_PRINTING : StatePrinting::Activate();
		         break;
		    case PAGE_MESSAGE :
		         break;
		    case PAGE_WIZARD :
		         break;
		    case PAGE_MENU :
		         break;
		    case PAGE_MOVEMENT : StateMovement::Activate(MODE_MOVE_AXIS);
		         break;
		    case PAGE_SETTINGS :
		         break;
		    case PAGE_ABOUT : StateAbout::Activate();
		         break;
		    case PAGE_WIZARDZ :
		         break;
		    case PAGE_C_NUMBER :
		         break;
		}
}

void NextionHMI::RaiseEvent(HMIevent event, uint8_t eventArg, const char *eventMsg) {
	lastEvent = event;
	lastEventArg = eventArg;
	//Error handling and common events
	switch(event) {
		case HMIevent::PRINTER_KILLED : StateMessage::ActivatePGM(MESSAGE_CRITICAL_ERROR, NEX_ICON_ERROR, eventMsg, PSTR(MSG_ERR_KILLED), 1, PSTR(RESTART_TO_CONTINUE), 0, 0, 0);
			return;
		case HMIevent::SWITCHED_OFF : StateMessage::ActivatePGM(MESSAGE_CRITICAL_ERROR, NEX_ICON_ERROR, eventMsg, PSTR(MSG_OFF), 1, PSTR(RESTART_TO_CONTINUE), 0, 0, 0);
			return;
		case HMIevent::TEMPERATURE_ERROR :
			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR("%s\\r%s %d"), eventMsg, MSG_STOPPED_HEATER, eventArg);
			StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, MSG_HEATER_ERROR, NextionHMI::buffer, 2, PSTR(MSG_OK), StateMessage::ReturnToLastState, PSTR(MSG_RETRY), StateMessage::RetryHeaterAndReturnToLastState);
			return;
		case HMIevent::SD_ERROR :
			if (eventArg!=0)
			{
				ZERO(NextionHMI::buffer);
				sprintf_P(NextionHMI::buffer, PSTR("%s %d"), eventMsg, eventArg);
				StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, NextionHMI::buffer, eventMsg, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
			}
			else
				StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, MSG_ERROR, eventMsg, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
			return;
		case HMIevent::ERROR :
			StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, MSG_ERROR, eventMsg, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
			return;
		case HMIevent::WAIT_FOR_INPUT :
			StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_INFO, eventMsg, PSTR(MSG_USERWAIT), 1, PSTR(MSG_OK), WaitOk_Push, 0, 0);
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
	}
}

uint8_t NextionHMI::GetActiveState() {
	return _pageID;
}

void NextionHMI::WaitOk_Push(void* ptr) {
    printer.setWaitForUser(false);
    StateMessage::ReturnToLastState(ptr);
}

void NextionHMI::UploadFirmwareFromSD() {
  if (IS_SD_INSERTED || card.cardOK) {
    Firmware.startUpload();
    nexSerial.end();
    Init();
  }
}

void NextionHMI::UploadFirmwareFromSerial(uint32_t tftSize) {
    Firmware.uploadFromSerial(tftSize);
    nexSerial.end();
    Init();
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

#if HAS_SDSUPPORT && PIN_EXISTS(SD_DETECT)
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

