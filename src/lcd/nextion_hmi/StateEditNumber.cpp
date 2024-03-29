/*
 * StateMessage.cpp
 *
 *  Created on: 9 ���. 2018 �.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateEditNumber.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_C_NUMBER,  0,  "c_num");

	//Content
	NexObject _vInput  = NexObject(PAGE_C_NUMBER,  18,  "c_num.input");
	NexObject _tShow  = NexObject(PAGE_C_NUMBER,  3,  "show");
	//Buttons
	NexObject _bOK = NexObject(PAGE_C_NUMBER,  4,  "bok");
	NexObject _bCancel = NexObject(PAGE_C_NUMBER,  21,  "bc");

	NexObject *_listenList[] = { &_bOK, &_bCancel, NULL };
}



void StateEditNumber::Activate(bool password, const char* input, NexTouchEventCb cbOK, NexTouchEventCb cbCancel) {

	_bOK.attachPush(cbOK);
	_bCancel.attachPush(cbCancel);
	_vInput.setText(input);


	NextionHMI::ActivateState(PAGE_C_NUMBER);
	_page.show();
	_tShow.setPw(password);

}

void StateEditNumber::TouchUpdate() {
	nexLoop(_listenList);
}

void StateEditNumber::GetInputToBuffer() {
	ZERO(NextionHMI::buffer);
	_vInput.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
}

void StateEditNumber::GetInputToBuffer(char *buffer, const uint16_t bufferSize) {
	_vInput.getText(buffer, bufferSize);
}




#endif

