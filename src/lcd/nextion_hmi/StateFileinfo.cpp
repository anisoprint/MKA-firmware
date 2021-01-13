/*
 * StateFileinfo.cpp
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateFileinfo.h"


namespace {

	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_FILEINFO,  0,  "fileinfo");

	NexObject _pFileicon  = NexObject(PAGE_FILEINFO,  9,  "pF");
	NexObject _tFilename  = NexObject(PAGE_FILEINFO,  7,  "tF");

	NexObject _tModified   = NexObject(PAGE_FILEINFO,  8,  "tM");
	NexObject _tPrintTime  = NexObject(PAGE_FILEINFO,  16,  "tT");

	NexObject _tPlastic    		= NexObject(PAGE_FILEINFO,  10,  "tP");
	NexObject _tComposite  		= NexObject(PAGE_FILEINFO,  13,  "tC");
	NexObject _tPlasticMat    	= NexObject(PAGE_FILEINFO,  11,  "tPP");
	NexObject _tCompPlasticMat  = NexObject(PAGE_FILEINFO,  14,  "tCP");
	NexObject _tCompFiberMat    = NexObject(PAGE_FILEINFO,  15,  "tCF");

	//Buttons
	NexObject _bPrint = NexObject(PAGE_FILEINFO,  1,  "ok");
	NexObject _bBack = NexObject(PAGE_FILEINFO,  6,  "cc");


	NexObject *_listenList[] = { &_bPrint, &_bBack, NULL };

	void FillFileInfo(PrintFileInfo fileInfo)
	{
		if (fileInfo.PrintDuration!=0)
		{
			char bufferTime[10] = {0};
			duration_t time = duration_t(fileInfo.PrintDuration);
			time.toDigital(bufferTime, false);

			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR(MSG_FILE_PRINT_TIME), bufferTime);
			_tPrintTime.setText(NextionHMI::buffer);
		}

		if (fileInfo.ExtruderInfo[0].PlasticConsumption!=0)
		{
			_tPlastic.setTextPGM(MSG_PLASTIC ":");

			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR(MSG_PLASTIC_CONS), fileInfo.ExtruderInfo[0].PlasticMaterialName, fileInfo.ExtruderInfo[0].PlasticConsumption);
			_tPlasticMat.setText(NextionHMI::buffer);
		}

		if (fileInfo.ExtruderInfo[1].PlasticConsumption!=0 && fileInfo.ExtruderInfo[1].FiberConsumption!=0)
		{
			_tComposite.setTextPGM(MSG_COMPOSITE ":");

			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR(MSG_PLASTIC_CONS), fileInfo.ExtruderInfo[1].PlasticMaterialName, fileInfo.ExtruderInfo[1].PlasticConsumption);
			_tCompPlasticMat.setText(NextionHMI::buffer);

			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR(MSG_FIBER_CONS), fileInfo.ExtruderInfo[1].FiberMaterialName, fileInfo.ExtruderInfo[1].FiberConsumption);
			_tCompFiberMat.setText(NextionHMI::buffer);
		}
	}
}

void StateFileinfo::Print_Push(void* ptr) {
	if (NextionHMI::pageData>=0) //SD
	{
	    commands.inject_rear_P(PSTR("M24"));
	}
	else //AC
	{
		if (netBridgeManager.SendJobInvoke(true, NextionHMI::buffer, NEXHMI_BUFFER_SIZE))
		{
			netBridgeManager.UpdateJobAwaiting(false);
			StateMessage::ActivatePGM_M(MESSAGE_DIALOG, NEX_ICON_INFO, MSG_AURA_CONNECT, PSTR(MSG_STARTING_JOB), 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
			return;
		}
		else
		{
			StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, MSG_ERROR, NextionHMI::buffer, 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
			return;
		}
	}

}

void StateFileinfo::Back_Push(void* ptr) {
	if (NextionHMI::pageData>=0) //SD
	{
		StateFiles::Activate(NextionHMI::pageData);
	}
	else //AC
	{
		StateStatus::Activate();
	}
}

void StateFileinfo::Init() {
	_bPrint.attachPush(Print_Push);
	_bBack.attachPush(Back_Push);
}

void StateFileinfo::Activate(int8_t sd_slot) {
	NextionHMI::ActivateState(PAGE_FILEINFO);
	_page.show();
	NextionHMI::pageData = sd_slot;

	//SD Card
	if (sd_slot>=0)
	{
		_tFilename.setText(sdStorage.cards[sd_slot].fileName);
		const char* auraString = PSTR("Aura");

		if (strstr_P(sdStorage.cards[sd_slot].generatedBy, auraString) != NULL)
		{
			_pFileicon.setPic(NEX_ICON_FILE_GCODE_AURA);
		}

		if (sdStorage.cards[sd_slot].fileModifiedDate!=0 && sdStorage.cards[sd_slot].fileModifiedDate!=0)
		{
			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR(MSG_FILE_MODIFIED_DATE), FAT_DAY(sdStorage.cards[sd_slot].fileModifiedDate), FAT_MONTH(sdStorage.cards[sd_slot].fileModifiedDate), FAT_YEAR(sdStorage.cards[sd_slot].fileModifiedDate), FAT_HOUR(sdStorage.cards[sd_slot].fileModifiedTime), FAT_MINUTE(sdStorage.cards[sd_slot].fileModifiedTime));
			_tModified.setText(NextionHMI::buffer);
		}

		FillFileInfo(sdStorage.cards[sd_slot].fileInfo);
	}
	else //AC Job
	{
		_tFilename.setText(netBridgeManager.serverJobName);
		_pFileicon.setPic(NEX_ICON_FILE_GCODE_AURA);

		FillFileInfo(netBridgeManager.serverJobInfo);
	}



}

void StateFileinfo::DrawUpdate() {
}

void StateFileinfo::TouchUpdate() {
	nexLoop(_listenList);
}

#endif

