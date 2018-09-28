/*
 * StatePrinting.cpp
 *
 *  Created on: 9 ���. 2018 �.
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
	NexObject _tStatus1  = NexObject(PAGE_PRINTING,  5,  "st1");
	NexObject _tStatus2  = NexObject(PAGE_PRINTING,  5,  "st2");
	NexObject _tProgress  = NexObject(PAGE_PRINTING,  11,  "perc");
	NexObject _pbProgressBar = NexObject(PAGE_PRINTING,  10,  "prb");
	//Buttons
	NexObject _bControl = NexObject(PAGE_PRINTING,  6,  "c");
	NexObject _bPause = NexObject(PAGE_PRINTING,  14,  "p");


	NexObject *_listenList[] = { &_bControl, &_bPause, NULL };

	int8_t _previousPercentDone = -1;
	int32_t _previousLayer = -1;
}

void StatePrinting::Cancel_Push(void* ptr) {
	StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_WARNING, PSTR(MSG_CANCEL_PRINTING), PSTR(MSG_CONFIRM_CANCEL_PRINTING), 2, PSTR(MSG_NO), CancelMessage_No, PSTR(MSG_YES), CancelMessage_Yes);
}

void StatePrinting::Pause_Push(void* ptr) {
    UNUSED(ptr);

    if (card.cardOK && card.isFileOpen()) {
      if (IS_SD_PRINTING) {
        card.pauseSDPrint();
        print_job_counter.pause();
        SERIAL_STR(PAUSE);
        SERIAL_EOL();
        _bPause.setTextPGM(PSTR(MSG_RESUME));
      }
      else {
        card.startFileprint();
        print_job_counter.start();
        _bPause.setTextPGM(PSTR(MSG_PAUSE));
      }
    }
}

void StatePrinting::OnEvent(HMIevent event, uint8_t eventArg) {
	switch(event) {
	    case HMIevent::HEATING_STARTED_BUILDPLATE :
	    case HMIevent::HEATING_STARTED_EXTRUDER :
	    	DrawUpdate();
	    	break;
	    case HMIevent::SD_PRINT_FINISHED :
	    	StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_FINISHED, PSTR(MSG_FINISHED), PSTR(MSG_DONE), 2, PSTR(MSG_OK), DoneMessage_OK, PSTR(MSG_PRINT_AGAIN), DoneMessage_Again, NEX_ICON_DONE);
	        break;
	    default:
	    	_tStatus1.setTextPGM(PSTR(MSG_PRINTING));
	}
}

void StatePrinting::Init() {
	_bControl.attachPush(Cancel_Push);
	_bPause.attachPush(Pause_Push);
}

void StatePrinting::Activate() {
	NextionHMI::ActivateState(PAGE_PRINTING);
	_page.show();

	_tFileName.setText(card.fileName);
	const char* auraString = PSTR("Aura");
	_previousPercentDone = -1;
	_previousLayer = -1;
	if (strstr_P(card.generatedBy, auraString) != NULL)
	{
		_pFileIcon.setPic(NEX_ICON_FILE_GCODE_AURA);
	}
	_tStatus1.setTextPGM(PSTR(MSG_PRINTING));
	DrawUpdate();
}

void StatePrinting::DrawUpdate() {
    if (IS_SD_PRINTING) {
    	switch(NextionHMI::lastEvent) {
    	    case HMIevent::HEATING_STARTED_BUILDPLATE :
    	    	 ZERO(NextionHMI::buffer);
    	    	 sprintf_P(NextionHMI::buffer, PSTR("%s (%d/%d\370C)"), MSG_BUILDPLATE_HEATING, (int)heaters[NextionHMI::lastEventArg].current_temperature, (int)heaters[NextionHMI::lastEventArg].target_temperature);
    		     _tStatus1.setText(NextionHMI::buffer);
    	    	 break;
    	    case HMIevent::HEATING_STARTED_EXTRUDER :
    			 if (NextionHMI::lastEventArg == HOT0_INDEX)
    			 {
    				ZERO(NextionHMI::buffer);
    				sprintf_P(NextionHMI::buffer, PSTR("%s (%d/%d\370C)"), MSG_PLASTIC_EXTRUDER_HEATING, (int)heaters[NextionHMI::lastEventArg].current_temperature, (int)heaters[NextionHMI::lastEventArg].target_temperature);
    			 }
				 else
				 {
					if (NextionHMI::lastEventArg == HOT1_INDEX)
					{
						ZERO(NextionHMI::buffer);
						sprintf_P(NextionHMI::buffer, PSTR("%s (%d/%d\370C)"), MSG_COMPOSITE_EXTRUDER_HEATING, (int)heaters[NextionHMI::lastEventArg].current_temperature, (int)heaters[NextionHMI::lastEventArg].target_temperature);
					}
				 }
   	    	     _tStatus1.setText(NextionHMI::buffer);
    	         break;
    	}
        if (_previousPercentDone != card.percentDone() || _previousLayer!=printer.currentLayer) {
        	  ZERO(NextionHMI::buffer);

        	  if (printer.currentLayer>0 && printer.maxLayer>0)
			  {
        		  sprintf_P(NextionHMI::buffer, PSTR("Layer: %d/%d - %d%%"), printer.currentLayer, printer.maxLayer, card.percentDone());
			  }
        	  else
        	  {
        		  sprintf_P(NextionHMI::buffer, PSTR("%d%%"), card.percentDone());
        	  }
			  _tProgress.setText(NextionHMI::buffer);
			  // Progress bar solid part
			  _pbProgressBar.setValue(card.percentDone());
			  _previousPercentDone = card.percentDone();
			  _previousLayer = printer.currentLayer;
        }
    }

}

void StatePrinting::TouchUpdate() {
	nexLoop(_listenList);
}

void StatePrinting::CancelMessage_Yes(void* ptr) {
	printer.setAbortSDprinting(true);
	StateStatus::Activate();
}

void StatePrinting::CancelMessage_No(void* ptr) {
	StatePrinting::Activate();
}

void StatePrinting::DoneMessage_OK(void* ptr) {
	StateStatus::Activate();
}

void StatePrinting::Control_Push(void* ptr) {
	StateMenu::ActivatePrintControl();
}

void StatePrinting::DoneMessage_Again(void* ptr) {
	String filename = String(card.fileName);
	card.selectFile(filename.c_str());
    commands.enqueue_and_echo_P(PSTR("M24"));
	StatePrinting::Activate();
}

#endif

