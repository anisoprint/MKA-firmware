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


void StatePrinting::Pause_Push(void* ptr) {
    UNUSED(ptr);

    if (card.cardOK && card.isFileOpen()) {

    	const float retract = PAUSE_PARK_RETRACT_LENGTH;

    	switch (PrintPause::Status)
    	{
			case NotPaused:
				PrintPause::PausePrint(retract);
				break;
			case WaitingToPause:
				PrintPause::ResumePrint();
				break;
			case Paused:
				PrintPause::ResumePrint(retract);
				break;
    	}


      /*if (IS_SD_PRINTING) {
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
      }*/
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
	        NextionHMI::RaiseEvent(NONE);
	    	break;
	    case HMIevent::PRINT_PAUSING :
			_bPause.setTextPGM(PSTR(MSG_PAUSING));
			_tStatus1.setTextPGM(PSTR(MSG_PAUSING));
			_tStatus2.setTextPGM(PSTR(""));
	    	break;
	    case HMIevent::PRINT_PAUSED :
	    	_bPause.setTextPGM(PSTR(MSG_RESUME));
	    	_tStatus1.setTextPGM(PSTR(MSG_PAUSED));
	    	_tStatus2.setTextPGM(PSTR(""));
	    	break;
	    case HMIevent::PRINT_PAUSE_SCHEDULED :
	    	_bPause.setTextPGM(PSTR(MSG_CANCEL_PAUSE));
	    	_tStatus1.setTextPGM(PSTR(MSG_WAITING_FOR_PAUSE));
	    	_tStatus2.setTextPGM(PSTR(MSG_PAUSE_DURING_FIBER));
	    	break;
	    case HMIevent::PRINT_PAUSE_UNSCHEDULED :
			_bPause.setTextPGM(PSTR(MSG_PAUSE));
			_tStatus1.setTextPGM(PSTR(MSG_PRINTING));
			_tStatus2.setTextPGM(PSTR(""));
			break;
	    case HMIevent::PRINT_PAUSE_RESUMING :
			_bPause.setTextPGM(PSTR(MSG_RESUMING));
			_tStatus1.setTextPGM(PSTR(MSG_RESUMING));
			_tStatus2.setTextPGM(PSTR(""));
	    	break;
	    case HMIevent::PRINT_PAUSE_RESUMED :
			_bPause.setTextPGM(PSTR(MSG_PAUSE));
			_tStatus1.setTextPGM(PSTR(MSG_PRINTING));
			_tStatus2.setTextPGM(PSTR(""));
	    	break;
	    case HMIevent::PRINT_CANCELLING :
			_bPause.setTextPGM(PSTR(MSG_CANCELLING));
			_tStatus1.setTextPGM(PSTR(MSG_CANCELLING));
	    	break;
	    case HMIevent::PRINT_CANCELLED:
	    	StateStatus::Activate();
	    	break;
	    default:
	    	_tStatus1.setTextPGM(PSTR(MSG_PRINTING));
	}
}

void StatePrinting::Init() {
	_bControl.attachPush(Control_Push);
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
	OnEvent(NextionHMI::lastEvent, NextionHMI::lastEventArg);
	DrawUpdate();

}

void StatePrinting::DrawUpdate() {
    	switch(NextionHMI::lastEvent) {
    	    case HMIevent::HEATING_STARTED_BUILDPLATE :
    	    	 ZERO(NextionHMI::buffer);
    	    	 sprintf_P(NextionHMI::buffer, PSTR(MSG_BUILDPLATE_HEATING), (int)heaters[NextionHMI::lastEventArg].current_temperature, (int)heaters[NextionHMI::lastEventArg].target_temperature);
    		     _tStatus1.setText(NextionHMI::buffer);
    	    	 break;
    	    case HMIevent::HEATING_STARTED_EXTRUDER :
    			 if (NextionHMI::lastEventArg == HOT0_INDEX)
    			 {
    				ZERO(NextionHMI::buffer);
    				sprintf_P(NextionHMI::buffer, PSTR(MSG_PLASTIC_EXTRUDER_HEATING), (int)heaters[NextionHMI::lastEventArg].current_temperature, (int)heaters[NextionHMI::lastEventArg].target_temperature);
    			 }
				 else
				 {
					if (NextionHMI::lastEventArg == HOT1_INDEX)
					{
						ZERO(NextionHMI::buffer);
						sprintf_P(NextionHMI::buffer, PSTR(MSG_COMPOSITE_EXTRUDER_HEATING), (int)heaters[NextionHMI::lastEventArg].current_temperature, (int)heaters[NextionHMI::lastEventArg].target_temperature);
					}
				 }
   	    	     _tStatus1.setText(NextionHMI::buffer);
    	         break;
    	}
        //if (IS_SD_PRINTING) {
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
    //}

}

void StatePrinting::TouchUpdate() {
	nexLoop(_listenList);
}

void StatePrinting::DoneMessage_OK(void* ptr) {
	StateStatus::Activate();
}

void StatePrinting::Control_Push(void* ptr) {
	if (PrintPause::Status!=Resuming)
	StateMenu::ActivatePrintControl();
}

void StatePrinting::DoneMessage_Again(void* ptr) {
	String filename = String(card.fileName);
	card.selectFile(filename.c_str());
    commands.enqueue_and_echo_P(PSTR("M24"));
	StatePrinting::Activate();
}

#endif

