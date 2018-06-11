/*
 * StateMovement.cpp
 *
 *  Created on: 10 θών. 2018 γ.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateStatus.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_MOVEMENT,  0,  "movement");

	//Variables
	NexObject  _gcode = NexObject(PAGE_MOVEMENT,  29,  "$gc");


	//Control
	NexObject _bMovementXplus  = NexObject(PAGE_MOVEMENT,  15,  "bXp");
	NexObject _bMovementXminus  = NexObject(PAGE_MOVEMENT,  12,  "bXm");
	NexObject _bMovementYplus  = NexObject(PAGE_MOVEMENT,  16,  "bYp");
	NexObject _bMovementYminus  = NexObject(PAGE_MOVEMENT,  13,  "bYm");
	NexObject _bMovementZplus  = NexObject(PAGE_MOVEMENT,  17,  "bZp");
	NexObject _bMovementZminus  = NexObject(PAGE_MOVEMENT,  14,  "bZm");

	NexObject _bMovementXhome  = NexObject(PAGE_MOVEMENT,  19,  "bXh");
	NexObject _bMovementYhome  = NexObject(PAGE_MOVEMENT,  18,  "bYh");
	NexObject _bMovementZhome  = NexObject(PAGE_MOVEMENT,  11,  "bZh");

	NexObject _tMovementX  = NexObject(PAGE_MOVEMENT,  26,  "tX");
	NexObject _tMovementY  = NexObject(PAGE_MOVEMENT,  27,  "tY");
	NexObject _tMovementZ  = NexObject(PAGE_MOVEMENT,  28,  "tZ");

	//Buttons
	NexObject _bMovementExtruders = NexObject(PAGE_MOVEMENT,  6,  "bE");
	NexObject _bMovementBack = NexObject(PAGE_MOVEMENT,  24,  "bB");

	NexObject *_listenList[] = { &_bMovementXplus, &_bMovementXminus,
			&_bMovementYplus, &_bMovementYminus, &_bMovementZplus,
			&_bMovementZminus, &_bMovementXhome, &_bMovementYhome, &_bMovementZhome,
			&_tMovementX, &_tMovementY, &_tMovementZ, &_bMovementExtruders,
			&_bMovementBack, NULL };

}
;

void StateMovement::Extruders_Push(void* ptr) {

}

void StateMovement::Back_Push(void* ptr) {
	StateMenu::ActivateMaintenance();
}

void StateMovement::Movement_Push(void* ptr) {
    ZERO(NextionHMI::buffer);
    _gcode.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
    commands.enqueue_and_echo_P(PSTR("G91"));
    commands.enqueue_and_echo(NextionHMI::buffer);
    commands.enqueue_and_echo_P(PSTR("G90"));
}

void StateMovement::Init() {
	_bMovementXplus.attachPush(Movement_Push);
	_bMovementXminus.attachPush(Movement_Push);

	_bMovementYplus.attachPush(Movement_Push);
	_bMovementYminus.attachPush(Movement_Push);

	_bMovementZplus.attachPush(Movement_Push);
	_bMovementZminus.attachPush(Movement_Push);

	_tMovementX.attachPush(Movement_Push);
	_tMovementY.attachPush(Movement_Push);
	_tMovementZ.attachPush(Movement_Push);

	_bMovementExtruders.attachPush(Extruders_Push);
	_bMovementBack.attachPush(Back_Push);
}

void StateMovement::Activate() {
	NextionHMI::ActivateState(PAGE_MOVEMENT);
	_page.show();
	DrawUpdate();
}

void StateMovement::DrawUpdate() {
	_tMovementX.setText(ftostr4sign(LOGICAL_X_POSITION(mechanics.current_position[X_AXIS])));
	_tMovementY.setText(ftostr4sign(LOGICAL_Y_POSITION(mechanics.current_position[Y_AXIS])));
	_tMovementZ.setText(ftostr4sign(LOGICAL_X_POSITION(mechanics.current_position[X_AXIS])));
}

void StateMovement::TouchUpdate() {
	nexLoop(_listenList);
}

#endif

