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
}

uint8_t NextionHMI::pageState = 0;
uint8_t NextionHMI::pageData = 0;

HMIevent NextionHMI::lastEvent = HMIevent::NONE;
uint8_t  NextionHMI::lastEventArg = 0;

NexObject NextionHMI::headerText = NexObject(0,  0,  "tH");
NexObject NextionHMI::headerIcon = NexObject(0,  0,  "iH");
NexObject NextionHMI::sdText = NexObject(0,  0,  "tSD");
NexObject NextionHMI::sdIcon = NexObject(0,  0,  "iSD");

char NextionHMI::buffer[70] = {0};

void NextionHMI::Init() {

	for (uint8_t i = 0; i < 10; i++) {
		ZERO(buffer);
		_nextionOn = nexInit(buffer);
		if (_nextionOn)
		{
			break;
		}
		HAL::delayMilliseconds(1000);
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
	StateTemperature::Init();
	StateFiles::Init();
	StateFileinfo::Init();
	StatePrinting::Init();
	StateMovement::Init();

	StateStatus::Activate();
}

void NextionHMI::DrawUpdate() {
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
	    case PAGE_PAUSE :
	         break;
	    case PAGE_CHANGE :
	         break;
	    case PAGE_WIZARD :
	         break;
	    case PAGE_MENU : //Maintenance_DrawUpdate();
	         break;
	    case PAGE_MOVEMENT :StateMovement::DrawUpdate();
	         break;
	    case PAGE_EXTRUDERS :
	         break;
	    case PAGE_SETTINGS :
	         break;
	    case PAGE_ABOUT :
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
	    case PAGE_PAUSE :
	         break;
	    case PAGE_CHANGE :
	         break;
	    case PAGE_WIZARD :
	         break;
	    case PAGE_MENU : StateMenu::TouchUpdate();
	         break;
	    case PAGE_MOVEMENT : StateMovement::TouchUpdate();
	         break;
	    case PAGE_EXTRUDERS :
	         break;
	    case PAGE_SETTINGS :
	         break;
	    case PAGE_ABOUT :
	         break;
	}
}

void NextionHMI::ActivateState(uint8_t state_id) {
	_pageID = state_id;
}

void NextionHMI::RaiseEvent(HMIevent event, uint8_t eventArg) {
	lastEvent = event;
	lastEventArg = eventArg;
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
	    case PAGE_PAUSE :
	         break;
	    case PAGE_CHANGE :
	         break;
	    case PAGE_WIZARD :
	         break;
	    case PAGE_MENU :
	         break;
	    case PAGE_MOVEMENT :
	         break;
	    case PAGE_EXTRUDERS :
	         break;
	    case PAGE_SETTINGS :
	         break;
	    case PAGE_ABOUT :
	         break;
	}
}

uint8_t NextionHMI::GetActiveState() {
	return _pageID;
}

#endif

