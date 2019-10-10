/*
 * StatePrinting.cpp
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StatePrinting.h"



//Settings Array
StateSettings::SettingDefinition StatePrinting::TuneList[] = {
			{MSG_MOVEMENT_FEEDRATE, StateSettings::SettingType::HEADER, nullptr, 0, 0},
			{" " MSG_FEEDRATE, StateSettings::SettingType::INT16, &mechanics.feedrate_percentage, 25, 200},

			{MSG_PLASTIC_FLOW_H, StateSettings::SettingType::HEADER, nullptr, 0, 0},
			{" " MSG_PLASTIC_FLOW, StateSettings::SettingType::INT16, &tools.flow_percentage[E_AXIS-XYZ], 25, 150},
			{" " MSG_COMP_PLASTIC_FLOW, StateSettings::SettingType::INT16, &tools.flow_percentage[V_AXIS-XYZ], 25, 150},
	};


namespace {

	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_PRINTING,  0,  "printing");

	NexObject _pFileIcon  = NexObject(PAGE_PRINTING,  7,  "ico");
	NexObject _tFileName  = NexObject(PAGE_PRINTING,  4,  "fnm");
	NexObject _tStatus1  = NexObject(PAGE_PRINTING,  5,  "st1");
	NexObject _tStatus2  = NexObject(PAGE_PRINTING,  5,  "st2");
	NexObject _tProgress  = NexObject(PAGE_PRINTING,  11,  "perc");
	NexObject _pbProgressBar = NexObject(PAGE_PRINTING,  10,  "prb");

	NexObject _tTempPlastic = NexObject(PAGE_PRINTING,  17,  "tT0");
	NexObject _tTempComposite = NexObject(PAGE_PRINTING,  19,  "tT1");
	NexObject _tTempBuildplate = NexObject(PAGE_PRINTING,  21,  "tTB");
	NexObject _tTempChamber = NexObject(PAGE_PRINTING,  22,  "tTC");
	NexObject _tFeedPercent = NexObject(PAGE_PRINTING,  25,  "tF");
	//Buttons
	NexObject _bControl = NexObject(PAGE_PRINTING,  6,  "c");
	NexObject _bPause = NexObject(PAGE_PRINTING,  14,  "p");


	NexObject *_listenList[] = { &_bControl, &_bPause, NULL };

	int8_t _previousProgress = -1;
	int32_t _previousLayer = -1;
	uint32_t _previousDuration = 0;

}


void StatePrinting::Pause_Push(void* ptr) {
    UNUSED(ptr);

    if (card.cardOK && card.isFileOpen()) {

    	const float retract = PrintPause::RetractDistance;

    	switch (PrintPause::Status)
    	{
			case NotPaused:
				//commands.enqueue_now_P(PSTR("M125"));
				PrintPause::PausePrint();
				break;
			case WaitingToPause:
				PrintPause::ResumePrint();
				break;
			case Paused:
				PrintPause::ResumePrint(retract);
				break;
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
	    	//_tStatus2.setTextPGM(PSTR(""));
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
			//_tStatus2.setTextPGM(PSTR(""));
	    	break;
	    case HMIevent::PRINT_PAUSE_RESUMED :
			_bPause.setTextPGM(PSTR(MSG_PAUSE));
			_tStatus1.setTextPGM(PSTR(MSG_PRINTING));
			//_tStatus2.setTextPGM(PSTR(""));
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
	//_tStatus2.refresh();
	//DrawUpdate();
	//_tStatus2.refresh();
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
	_previousProgress = -1;
	_previousLayer = -1;
	if (strstr_P(card.generatedBy, auraString) != NULL)
	{
		_pFileIcon.setPic(NEX_ICON_FILE_GCODE_AURA);
	}

	switch(PrintPause::Status)
	{
	case WaitingToPause:
		OnEvent(HMIevent::PRINT_PAUSE_SCHEDULED, 0);
		break;
	case Pausing:
		OnEvent(HMIevent::PRINT_PAUSING, 0);
		break;
	case Paused:
		OnEvent(HMIevent::PRINT_PAUSED, 0);
		break;
	case Resuming:
		OnEvent(HMIevent::PRINT_PAUSE_RESUMING, 0);
		break;
	default:
		OnEvent(NextionHMI::lastEvent, NextionHMI::lastEventArg);
		break;
	}


	DrawUpdate();

}

void StatePrinting::DrawUpdate() {
    	switch(NextionHMI::lastEvent) {
    	    case HMIevent::HEATING_STARTED_BUILDPLATE :
    	    	 ZERO(NextionHMI::buffer);
    	    	 sprintf_P(NextionHMI::buffer, PSTR(MSG_BUILDPLATE_HEATING), round(heaters[NextionHMI::lastEventArg].current_temperature), (int)heaters[NextionHMI::lastEventArg].target_temperature);
    		     _tStatus1.setText(NextionHMI::buffer);
    	    	 break;
    	    case HMIevent::HEATING_STARTED_EXTRUDER :
    			 if (NextionHMI::lastEventArg == HOT0_INDEX)
    			 {
    				ZERO(NextionHMI::buffer);
    				sprintf_P(NextionHMI::buffer, PSTR(MSG_PLASTIC_EXTRUDER_HEATING), round(heaters[NextionHMI::lastEventArg].current_temperature), (int)heaters[NextionHMI::lastEventArg].target_temperature);
    			 }
				 else
				 {
					if (NextionHMI::lastEventArg == HOT1_INDEX)
					{
						ZERO(NextionHMI::buffer);
						sprintf_P(NextionHMI::buffer, PSTR(MSG_COMPOSITE_EXTRUDER_HEATING), round(heaters[NextionHMI::lastEventArg].current_temperature), (int)heaters[NextionHMI::lastEventArg].target_temperature);
					}
				 }
   	    	     _tStatus1.setText(NextionHMI::buffer);
    	         break;
    	}
        //if (IS_SD_PRINTING) {
        if (_previousProgress != printer.progress || _previousLayer!=printer.currentLayer) {
        	  ZERO(NextionHMI::buffer);

        	  if (printer.currentLayer>0 && printer.maxLayer>0)
			  {
        		  sprintf_P(NextionHMI::buffer, PSTR(MSG_LAYER_NUMBER), printer.currentLayer, printer.maxLayer, printer.progress);
			  }
        	  else
        	  {
        		  sprintf_P(NextionHMI::buffer, PSTR("%d%%"), printer.progress);
        	  }
			  _tProgress.setText(NextionHMI::buffer);
			  // Progress bar solid part
			  _pbProgressBar.setValue(printer.progress);

			  if (_previousProgress != printer.progress) _previousDuration = print_job_counter.duration();

			  _previousProgress = printer.progress;
			  _previousLayer = printer.currentLayer;
        }

		if (PrintPause::Status!=PrintPauseStatus::WaitingToPause)
		{
			// Estimate End Time
			ZERO(NextionHMI::buffer);
			char bufferElapsed[10];
			char bufferLeft[10];

			uint8_t digit;
			duration_t time = duration_t(print_job_counter.duration());
			time.toDigital(bufferElapsed, false);
			time = (printer.progress > 0) ? duration_t(_previousDuration * (100 - printer.progress) / (printer.progress + 0.1)) : duration_t(0);
			time.toDigital(bufferLeft, false);
			sprintf_P(NextionHMI::buffer, PSTR(MSG_PRINTING_TIME), bufferElapsed, bufferLeft);

			_tStatus2.setText(NextionHMI::buffer);
		  }

    	auto strTemp = String(round(heaters[HOT0_INDEX].current_temperature)) + "\370C";
        _tTempPlastic.setText(strTemp.c_str());
        strTemp = String(round(heaters[HOT1_INDEX].current_temperature)) + "\370C";
        _tTempComposite.setText(strTemp.c_str());
        strTemp = String(round(heaters[BED_INDEX].current_temperature)) + "\370C";
        _tTempBuildplate.setText(strTemp.c_str());

#if HAS_HEATER_CHAMBER
        if (heaters[CHAMBER_INDEX].current_temperature>0)
		{
			strTemp = String(round(heaters[CHAMBER_INDEX].current_temperature)) + "\370C";
		}
		else
		{
			strTemp = String("-");
		}
        _tTempChamber.setText(strTemp.c_str());
#endif
        strTemp = String(mechanics.feedrate_percentage) + "%";
        _tFeedPercent.setText(strTemp.c_str());

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
    commands.inject_rear_P(PSTR("M24"));
	StatePrinting::Activate();
}

#endif

