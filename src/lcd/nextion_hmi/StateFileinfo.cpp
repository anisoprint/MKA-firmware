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

	//Buttons
	NexObject _bPrint = NexObject(PAGE_FILEINFO,  1,  "ok");
	NexObject _bBack = NexObject(PAGE_FILEINFO,  6,  "cc");


	NexObject *_listenList[] = { &_bPrint, &_bBack, NULL };
}

void StateFileinfo::Print_Push(void* ptr) {
    commands.enqueue_and_echo_P(PSTR("M24"));
}

void StateFileinfo::Back_Push(void* ptr) {
	StateFiles::Activate();
}

void StateFileinfo::Init() {
	_bPrint.attachPush(Print_Push);
	_bBack.attachPush(Back_Push);
}

void StateFileinfo::Activate() {
	NextionHMI::ActivateState(PAGE_FILEINFO);
	_page.show();
	_tFilename.setText(card.fileName);
	const char* auraString = PSTR("Aura");
	if (strstr_P(card.generatedBy, auraString) != NULL)
	{
		_pFileicon.setPic(NEX_ICON_FILE_GCODE_AURA);
	}
}

void StateFileinfo::DrawUpdate() {
}

void StateFileinfo::TouchUpdate() {
	nexLoop(_listenList);
}

#endif

