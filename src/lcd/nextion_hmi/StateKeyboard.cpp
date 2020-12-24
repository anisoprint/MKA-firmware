/*
 * StateMessage.cpp
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateKeyboard.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_C_KEY,  0,  "c_key");

	//Content
	NexObject _tHeader  = NexObject(PAGE_C_KEY,  43,  "tHeader");
	NexObject _vInput  = NexObject(PAGE_C_KEY,  37,  "c_key.input");
	NexObject _tShow  = NexObject(PAGE_C_KEY,  38,  "show");
	//Buttons
	NexObject _bOK = NexObject(PAGE_C_KEY,  4,  "b210");
	NexObject _bCancel = NexObject(PAGE_C_KEY,  3,  "b251");

	NexObject *_listenList[] = { &_bOK, &_bCancel, NULL };
}



void StateKeyboard::Activate(bool password, const char* header, const char* input, NexTouchEventCb cbOK, NexTouchEventCb cbCancel) {

	_bOK.attachPush(cbOK);
	_bCancel.attachPush(cbCancel);
	_vInput.setText(input);

	NextionHMI::ActivateState(PAGE_C_KEY);
	_page.show();
	_tShow.setPw(password);
	_tHeader.setText(header);

}

void StateKeyboard::TouchUpdate() {
	nexLoop(_listenList);
}

void StateKeyboard::GetInputToBuffer() {
	ZERO(NextionHMI::buffer);
	_vInput.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
}


void StateKeyboard::GetInputToBuffer(char *buffer, const uint16_t bufferSize) {
	_vInput.getText(buffer, bufferSize);
}

#endif

