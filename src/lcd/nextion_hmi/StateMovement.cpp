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
	bool _moveMode = false;

	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_MOVEMENT,  0,  "movement");

	//Variables
	NexObject  _gcode = NexObject(PAGE_MOVEMENT,  29,  "$gc");
	NexObject  _mode = NexObject(PAGE_MOVEMENT,  29,  "movement.$md");

	//Control
	NexObject _bMovementAplus  = NexObject(PAGE_MOVEMENT,  15,  "bAp");
	NexObject _bMovementAminus  = NexObject(PAGE_MOVEMENT,  12,  "bAm");
	NexObject _bMovementBplus  = NexObject(PAGE_MOVEMENT,  16,  "bBp");
	NexObject _bMovementBminus  = NexObject(PAGE_MOVEMENT,  13,  "bBm");
	NexObject _bMovementCplus  = NexObject(PAGE_MOVEMENT,  17,  "bCp");
	NexObject _bMovementCminus  = NexObject(PAGE_MOVEMENT,  14,  "bCm");

	NexObject _bMovementAact  = NexObject(PAGE_MOVEMENT,  19,  "bAa");
	NexObject _bMovementBact  = NexObject(PAGE_MOVEMENT,  18,  "bBa");
	NexObject _bMovementCact  = NexObject(PAGE_MOVEMENT,  11,  "bCa");

	NexObject _tMovementA  = NexObject(PAGE_MOVEMENT,  26,  "tA");
	NexObject _tMovementB  = NexObject(PAGE_MOVEMENT,  27,  "tB");
	NexObject _tMovementC  = NexObject(PAGE_MOVEMENT,  28,  "tC");

	//Buttons
	NexObject _bMovementMode = NexObject(PAGE_MOVEMENT,  6,  "bM");
	NexObject _bMovementBack = NexObject(PAGE_MOVEMENT,  24,  "bB");

	NexObject *_listenList[] = { &_bMovementAplus, &_bMovementAminus,
			&_bMovementBplus, &_bMovementBminus, &_bMovementCplus,
			&_bMovementCminus, &_bMovementAact, &_bMovementBact, &_bMovementCact,
			&_tMovementA, &_tMovementB, &_tMovementC, &_bMovementMode,
			&_bMovementBack, NULL };

}
;

void StateMovement::Extruders_Push(void* ptr) {
	StateMovement::Activate(!_moveMode);
}

void StateMovement::Back_Push(void* ptr) {
	StateMenu::ActivateMaintenance();
}

void StateMovement::Movement_Push(void* ptr) {
	if (_moveMode==MODE_MOVE_EXTRUDERS && ptr==&_bMovementBact)
	{
		//cut
		tools.cut_fiber();
	}
	else
	{
		ZERO(NextionHMI::buffer);
		_gcode.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
		if (_moveMode==MODE_MOVE_EXTRUDERS)
		{
			if (((ptr==&_bMovementAplus || ptr==&_bMovementAminus) && thermalManager.tooColdToExtrude(0)) ||
				((ptr==&_bMovementBplus || ptr==&_bMovementBminus) && thermalManager.tooColdToExtrude(1)) ||
				((ptr==&_bMovementCplus || ptr==&_bMovementCminus) && thermalManager.tooColdToExtrude(1)))
			{
				StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_WARNING, PSTR(MSG_COLD_HOTEND), PSTR(MSG_COLD_HOTEND_TEXT), 1, PSTR(MSG_OK), (NexTouchEventCb)StateMovement::ActivateExtruders, NULL, NULL, 0);
				return;
			}
		}
		commands.enqueue_and_echo_P(PSTR("G91"));
		commands.enqueue_and_echo(NextionHMI::buffer);
		commands.enqueue_and_echo_P(PSTR("G90"));
	}
	DrawUpdate();
}

void StateMovement::Init() {
	_bMovementAplus.attachPush(Movement_Push, &_bMovementAplus);
	_bMovementAminus.attachPush(Movement_Push, &_bMovementAminus);

	_bMovementBplus.attachPush(Movement_Push, &_bMovementBplus);
	_bMovementBminus.attachPush(Movement_Push, &_bMovementBminus);

	_bMovementCplus.attachPush(Movement_Push, &_bMovementCplus);
	_bMovementCminus.attachPush(Movement_Push, &_bMovementCminus);

	_bMovementAact.attachPush(Movement_Push, &_bMovementAact);
	_bMovementBact.attachPush(Movement_Push, &_bMovementBact);
	_bMovementCact.attachPush(Movement_Push, &_bMovementCact);

	_bMovementMode.attachPush(Extruders_Push);
	_bMovementBack.attachPush(Back_Push);
}

void StateMovement::Activate(bool mode) {
	_moveMode = mode;
	_mode.setValue(_moveMode);
	NextionHMI::ActivateState(PAGE_MOVEMENT);
	_page.show();
	DrawUpdate();
}

void StateMovement::ActivateExtruders() {
	Activate(MODE_MOVE_EXTRUDERS);
}

void StateMovement::DrawUpdate() {
	if (_moveMode==MODE_MOVE_AXIS)
	{
		ZERO(NextionHMI::buffer);
		sprintf_P(NextionHMI::buffer, PSTR("%.2f"), LOGICAL_X_POSITION(mechanics.current_position[X_AXIS]));
		_tMovementA.setText(NextionHMI::buffer);

		ZERO(NextionHMI::buffer);
		sprintf_P(NextionHMI::buffer, PSTR("%.2f"), LOGICAL_Y_POSITION(mechanics.current_position[Y_AXIS]));
		_tMovementB.setText(NextionHMI::buffer);

		ZERO(NextionHMI::buffer);
		sprintf_P(NextionHMI::buffer, PSTR("%.2f"), LOGICAL_Z_POSITION(mechanics.current_position[Z_AXIS]));
		_tMovementC.setText(NextionHMI::buffer);
	}
	else
	{
			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR("%.2f"), mechanics.current_position[E_AXIS]);
			_tMovementA.setText(NextionHMI::buffer);

			ZERO(NextionHMI::buffer);
			sprintf_P(NextionHMI::buffer, PSTR("%.2f"), mechanics.current_position[U_AXIS]);
			_tMovementB.setText(NextionHMI::buffer);

#if DRIVER_EXTRUDERS>2
		ZERO(NextionHMI::buffer);
		sprintf_P(NextionHMI::buffer, PSTR("%.2f"), mechanics.current_position[V_AXIS]);
		_tMovementC.setText(NextionHMI::buffer);
#endif
	}
}

void StateMovement::TouchUpdate() {
	nexLoop(_listenList);
}



#endif

