/*
 * StateAbout.cpp
 *
 *  Created on: 5 ???. 2018 ?.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

namespace {

	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_ABOUT,  0,  "about");

	NexObject _tName  = NexObject(PAGE_ABOUT,  7,  "tN");
	NexObject _tSerial  = NexObject(PAGE_ABOUT,  14,  "tSN");
	NexObject _tFirmware  = NexObject(PAGE_ABOUT,  11,  "tFW");
	NexObject _tBuild  = NexObject(PAGE_ABOUT,  12,  "tB");

	//Buttons
	NexObject _bBack = NexObject(PAGE_ABOUT,  6,  "bB");

	NexObject *_listenList[] = { &_bBack, NULL };
}


void StateAbout::Back_Push(void* ptr) {
	StateMenu::ActivateMaintenance();
}

void StateAbout::Init() {
	_bBack.attachPush(Back_Push);
}

void StateAbout::Activate() {
	NextionHMI::ActivateState(PAGE_ABOUT);
	_page.show();

	ZERO(NextionHMI::buffer);
	sprintf_P(NextionHMI::buffer, PSTR(MSG_PRINTER_FULL_NAME), PSTR(CUSTOM_MACHINE_NAME), eeprom.printerVersion);
	_tName.setText(NextionHMI::buffer);
	_tSerial.setText(eeprom.printerSN);
	_tFirmware.setText(FIRMWARE_NAME " " SHORT_BUILD_VERSION);
	_tBuild.setText(__DATE__);


}

void StateAbout::TouchUpdate() {
	nexLoop(_listenList);
}

#endif

