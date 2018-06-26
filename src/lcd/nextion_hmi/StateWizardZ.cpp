/*
 * StateWizardZ.cpp
 *
 *  Created on: 26 θών. 2018 γ.
 *      Author: Azarov
 */



#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateWizardZ.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_WIZARDZ,  0,  "wizardz");

	//Variables
	NexObject  _gcode = NexObject(PAGE_WIZARDZ,  29,  "$gc");


	//Control
	NexObject _bMovement1  = NexObject(PAGE_WIZARDZ,  10,  "bd1");
	NexObject _bMovement2  = NexObject(PAGE_WIZARDZ,  11,  "bd2");
	NexObject _bMovement3  = NexObject(PAGE_WIZARDZ,  12,  "bd3");
	NexObject _bMovement4  = NexObject(PAGE_WIZARDZ,  13,  "bu1");
	NexObject _bMovement5  = NexObject(PAGE_WIZARDZ,  14,  "bu2");
	NexObject _bMovement6  = NexObject(PAGE_WIZARDZ,  15,  "bu3");

	//Buttons
	NexObject _bLeft =  NexObject(PAGE_WIZARDZ,  9,  "bL");
	NexObject _bRight = NexObject(PAGE_WIZARDZ,  1,  "bR");

	NexObject *_listenList[] = { &_bMovement1, &_bMovement2,
			&_bMovement3, &_bMovement4, &_bMovement5,
			&_bMovement6, &_bLeft, &_bRight, NULL };

}
;

void StateWizardZ::Movement_Push(void* ptr) {
    ZERO(NextionHMI::buffer);
    _gcode.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    commands.enqueue_and_echo_P(PSTR("G91"));
    commands.enqueue_and_echo(NextionHMI::buffer);
    commands.enqueue_and_echo_P(PSTR("G90"));
}

void StateWizardZ::Init() {
	_bMovement1.attachPush(Movement_Push);
	_bMovement2.attachPush(Movement_Push);
	_bMovement3.attachPush(Movement_Push);
	_bMovement4.attachPush(Movement_Push);
	_bMovement5.attachPush(Movement_Push);
	_bMovement6.attachPush(Movement_Push);
}

void StateWizardZ::TouchUpdate() {
	nexLoop(_listenList);
}

#endif


