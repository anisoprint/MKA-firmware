/*
 * StateWizard.cpp
 *
 *  Created on: 26 θών. 2018 γ.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateWizard.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_WIZARD,  0,  "wizard");

	//Variables
	NexObject _head = NexObject(PAGE_WIZARD,  14,  "wizard.$h");
	NexObject _pic  = NexObject(PAGE_WIZARD,  23,   "wizard.$pic");
	NexObject _text = NexObject(PAGE_WIZARD,  12,  "wizard.$text");
	NexObject _buttonsNum = NexObject(PAGE_WIZARD,  15,  "wizard.$bn");

	//Buttons
	NexObject _bL = NexObject(PAGE_MENU,  10,   "b1");
	NexObject _bR = NexObject(PAGE_MENU,  2,  "b2");
	NexObject _bΡ = NexObject(PAGE_MENU,  16,   "b3");

	NexObject *_listenList[] = { &_bL, &_bR, &_bC, NULL };


}

void StateWizard::TouchUpdate() {
	nexLoop(_listenList);
}

void StateWizard::ZOffsetS0(void* ptr) {

	NextionHMI::ActivateState(PAGE_WIZARD);
	_buttonsNum.setValue(2);
	_pic.setValue(0);

	_head.setTextPGM(PSTR(MSG_HEADER_Z_OFFSET));
	_text.setTextPGM(PSTR(MSG_Z_OFFSET_S1));

	_page.show();
	NextionHMI::headerText.setTextPGM(PSTR(MSG_HEADER_Z_OFFSET));
	NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

	_bL.setTextPGM(PSTR(MSG_CANCEL));
	_bR.setTextPGM(PSTR(MSG_NEXT));

	_bL.attachPush(ZOffsetCancel);
	_bR.attachPush(StateWizardZ::ZOffsetS1);
}


#endif

