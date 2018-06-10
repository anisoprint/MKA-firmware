/*
 * StateMessage.cpp
 *
 *  Created on: 9 ���. 2018 �.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateMessage.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_MESSAGE,  0,  "message");

	//Content
	NexObject _pPicture  = NexObject(PAGE_MESSAGE,  8,  "pP");
	NexObject _tMessage  = NexObject(PAGE_MESSAGE,  5,  "tM");
	//Buttons
	NexObject _bLeft = NexObject(PAGE_MESSAGE,  9,  "bL");
	NexObject _bRight = NexObject(PAGE_MESSAGE,  10,  "bR");
	NexObject _bSingle = NexObject(PAGE_MESSAGE,  1,  "bS");

	NexObject *_listenList[] = { &_bLeft, &_bRight, &_bSingle, NULL };

	uint8_t _currentPriority = 0;
}
;

void StateMessage::Init() {
}

void StateMessage::ActivatePGM(uint8_t priority, uint8_t icon,
		const char* header_P, const char* message_P, uint8_t numButtons,
		const char* txtButtonRight_P, NexTouchEventCb cbRight,
		const char* txtButtonLeft_P, NexTouchEventCb cbLeft, uint8_t picture) {

	if (NextionHMI::GetActiveState() == PAGE_MESSAGE && priority<_currentPriority) return;

	NextionHMI::ActivateState(PAGE_MESSAGE);
	_page.show();



	if(numButtons==1)
	{
		_bLeft.SetVisibility(false);
		_bRight.SetVisibility(false);
		_bSingle.setTextPGM(txtButtonRight_P);
		_bSingle.attachPush(cbRight);
	}
	else
	{
		_bLeft.setTextPGM(txtButtonLeft_P);
		_bLeft.attachPush(cbLeft);
		_bRight.setTextPGM(txtButtonRight_P);
		_bRight.attachPush(cbRight);
	}

	NextionHMI::headerIcon.setPic(icon);
	NextionHMI::headerText.setTextPGM(header_P);

	if (picture!=0)
	{
		_pPicture.setPic(picture);
		_pPicture.SetVisibility(true);
	}

	_tMessage.setTextPGM(message_P);

	_currentPriority = priority;

}

void StateMessage::TouchUpdate() {
	nexLoop(_listenList);
}

#endif

