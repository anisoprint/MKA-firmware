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
	NexObject _bC = NexObject(PAGE_MENU,  16,   "b3");

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

	Init2Buttons(PSTR(MSG_CANCEL), ZOffsetCancel, PSTR(MSG_NEXT), StateWizardZ::ZOffsetS1);

}

void StateWizard::ZOffsetFinish(void* ptr) {
}

void StateWizard::ZOffsetCancel(void* ptr) {
}

inline void StateWizard::Init2Buttons(const char* txtLeft, NexTouchEventCb cbLeft,
		const char* txtRight, NexTouchEventCb cbRight) {
	_bL.setTextPGM(txtLeft);
	_bR.setTextPGM(txtRight);
	_bL.attachPush(cbLeft);
	_bR.attachPush(cbRight);
}

inline void StateWizard::Init1Button(const char* txtCenter, NexTouchEventCb cbCenter) {
	_bC.setTextPGM(PSTR(txtCenter));
	_bC.attachPush(cbCenter);
}

#endif

