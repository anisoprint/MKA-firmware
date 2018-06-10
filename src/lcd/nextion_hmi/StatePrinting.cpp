/*
 * StatePrinting.cpp
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StatePrinting.h"


namespace {

	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_PRINTING,  0,  "printing");

	//Plastic
	NexObject _pFileIcon  = NexObject(PAGE_PRINTING,  7,  "ico");
	NexObject _tFileName  = NexObject(PAGE_PRINTING,  4,  "fnm");
	NexObject _tStatus  = NexObject(PAGE_PRINTING,  5,  "sta");
	NexObject _tProgress  = NexObject(PAGE_PRINTING,  11,  "perc");
	NexObject _pbProgressBar = NexObject(PAGE_PRINTING,  10,  "prb");
	//Buttons
	NexObject _bCancel = NexObject(PAGE_PRINTING,  14,  "cc");
	NexObject _bPause = NexObject(PAGE_PRINTING,  6,  "p");


	NexObject *_listenList[] = { &_bCancel, &_bPause, NULL };

	int8_t _previousPercentDone = -1;
}

void StatePrinting::Cancel_Push(void* ptr) {
	StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_WARNING, MSG_CANCEL_PRINTING, MSG_CONFIRM_CANCEL_PRINTING, 2, MSG_NO, CancelMessage_No, MSG_YES, CancelMessage_Yes);
}

void StatePrinting::Pause_Push(void* ptr) {
}

void StatePrinting::OnEvent(HMIevent event, uint8_t eventArg) {
	switch(event) {
	    case HMIevent::HEATING_STARTED_BUILDPLATE :
	    case HMIevent::HEATING_STARTED_EXTRUDER :
	    	DrawUpdate();
	    	break;
	    default:
	    	_tStatus.setTextPGM(PSTR(MSG_PRINTING));
	}
}

void StatePrinting::Init() {
	_bCancel.attachPush(Cancel_Push);
	_bPause.attachPush(Pause_Push);
}

void StatePrinting::Activate() {
	NextionHMI::ActivateState(PAGE_PRINTING);
	_page.show();

	_tFileName.setText(card.fileName);
	const char* auraString = PSTR("Aura");
	_previousPercentDone = -1;
	if (strstr_P(card.generatedBy, auraString) != NULL)
	{
		_pFileIcon.setPic(NEX_ICON_FILE_GCODE_AURA);
	}
	_tStatus.setTextPGM(PSTR(MSG_PRINTING));
	DrawUpdate();
}

void StatePrinting::DrawUpdate() {
    if (IS_SD_PRINTING) {
  	  serial_print("\n>>>>>>>>>>>>\n");
  	  serial_print("1");
  	  serial_print("\n>>>>>>>>>>>>\n");
    	switch(NextionHMI::lastEvent) {
    	    case HMIevent::HEATING_STARTED_BUILDPLATE :
    	    	 sprintf_P(NextionHMI::buffer, PSTR("%s (%d/%d\370C)"), MSG_BUILDPLATE_HEATING, (int)heaters[NextionHMI::lastEventArg].current_temperature, (int)heaters[NextionHMI::lastEventArg].target_temperature);
    		     _tStatus.setText(NextionHMI::buffer);
    	    	 break;
    	    case HMIevent::HEATING_STARTED_EXTRUDER :
    			 if (NextionHMI::lastEventArg == HOT0_INDEX)
    			 {
    				sprintf_P(NextionHMI::buffer, PSTR("%s (%d/%d\370C)"), MSG_PLASTIC_EXTRUDER_HEATING, (int)heaters[NextionHMI::lastEventArg].current_temperature, (int)heaters[NextionHMI::lastEventArg].target_temperature);
    			 }
				 else
				 {
					if (NextionHMI::lastEventArg == HOT1_INDEX)
					{
						sprintf_P(NextionHMI::buffer, PSTR("%s (%d/%d\370C)"), MSG_COMPOSITE_EXTRUDER_HEATING, (int)heaters[NextionHMI::lastEventArg].current_temperature, (int)heaters[NextionHMI::lastEventArg].target_temperature);
					}
				 }
   	    	     _tStatus.setText(NextionHMI::buffer);
				  serial_print("\n>>>>>>>>>>>>\n");
				  serial_print("2");
				  serial_print("\n>>>>>>>>>>>>\n");
    	         break;
    	}
        if (_previousPercentDone != card.percentDone()) {
			  serial_print("\n>>>>>%>>>>>>\n");
			  serial_print(card.percentDone());
			  serial_print("\n>>>>>%>>>>>>\n");
			  sprintf_P(NextionHMI::buffer, PSTR("%d%%"), card.percentDone());
			  _tProgress.setText(NextionHMI::buffer);
			  // Progress bar solid part
			  _pbProgressBar.setValue(card.percentDone());
			  _previousPercentDone = card.percentDone();
        }
    }

}

void StatePrinting::TouchUpdate() {
	nexLoop(_listenList);
}

void StatePrinting::CancelMessage_Yes(void* ptr) {
	card.stopSDPrint();
	StateStatus::Activate();
}

void StatePrinting::CancelMessage_No(void* ptr) {
	StatePrinting::Activate();
}

void StatePrinting::DoneMessage_OK(void* ptr) {
}

void StatePrinting::DoneMessage_Again(void* ptr) {
}

#endif

