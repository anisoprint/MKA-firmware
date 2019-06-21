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
	///////////// Nextion components /////////////
	//Page
	NexObject _page = NexObject(PAGE_WIZARDZ,  0,  "wizardz");

	//Variables
	NexObject  _gcode = NexObject(PAGE_WIZARDZ,  29,  "$gc");

	NexObject  _txtHeader = NexObject(PAGE_WIZARDZ,  3,  "tT");
	NexObject  _txtCaption = NexObject(PAGE_WIZARDZ,  2,  "tC");
	NexObject  _txtZ = NexObject(PAGE_WIZARDZ,  20,  "tZ");

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

};

void StateWizardZ::Movement_Push(void* ptr) {
	if (planner.movesplanned()<4)
	{
		ZERO(NextionHMI::buffer);
		_gcode.getText(NextionHMI::buffer, sizeof(NextionHMI::buffer));
		double dz = strtod(NextionHMI::buffer, NULL);
        mechanics.current_position[Z_AXIS] += dz;
        planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);
	}
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

void StateWizardZ::ZOffsetS2(void* ptr) {

	if (!planner.movesplanned())
	{
		//Going to center adjust position
		mechanics.do_blocking_move_to_xy(int(BED_CENTER_X), int(BED_CENTER_Y), NOZZLE_PARK_XY_FEEDRATE);

		//Going to Z adjust position
		mechanics.do_blocking_move_to_z(10, mechanics.homing_feedrate_mm_s[Z_AXIS]);

		NextionHMI::ActivateState(PAGE_WIZARDZ);

		_page.show();
		NextionHMI::headerText.setTextPGM(PSTR(MSG_HEADER_Z_OFFSET));
		NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);
		_txtHeader.setTextPGM(PSTR(MSG_HEADER_Z_OFFSET ": 2/2"));
		_txtCaption.setTextPGM(PSTR(MSG_Z_OFFSET_ST2));

		_bLeft.setTextPGM(PSTR(MSG_CANCEL));
		_bRight.setTextPGM(PSTR(MSG_FINISH));

		_bLeft.attachPush(StateWizard::ZAxisCancel);
		_bRight.attachPush(StateWizard::ZAxisFinish);

		DrawUpdate();

	}

}

void StateWizardZ::BuildPlateS3(void* ptr) {
	//SERIAL_VAL(planner.movesplanned());
	//SERIAL_EOL();
	if (!planner.movesplanned())
	{
		NextionHMI::ActivateState(PAGE_WIZARDZ);

		_page.show();
		NextionHMI::headerText.setTextPGM(PSTR(MSG_HEADER_BP_CALIBR));
		NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);
		_txtHeader.setTextPGM(PSTR(MSG_HEADER_BP_CALIBR ": 3/9"));
		_txtCaption.setTextPGM(PSTR(MSG_BP_CALIBR_ST3));

		_bLeft.setTextPGM(PSTR(MSG_CANCEL));
		_bRight.setTextPGM(PSTR(MSG_NEXT));

		_bLeft.attachPush(StateWizard::BuildPlateCancel);
		_bRight.attachPush(StateWizard::BuildPlateS4);

		DrawUpdate();

	}
}


void StateWizardZ::DrawUpdate() {

	ZERO(NextionHMI::buffer);
	sprintf_P(NextionHMI::buffer, PSTR("dZ = %.2f"), mechanics.current_position[Z_AXIS]-LEVELING_OFFSET);

	_txtZ.setText(NextionHMI::buffer);
}

void StateWizardZ::BuildPlateS7(void* ptr) {
	if (!planner.movesplanned())
	{
		NextionHMI::ActivateState(PAGE_WIZARDZ);

		_page.show();
		NextionHMI::headerText.setTextPGM(PSTR(MSG_HEADER_BP_CALIBR));
		NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);
		_txtHeader.setTextPGM(PSTR(MSG_HEADER_BP_CALIBR ": 7/9"));
		_txtCaption.setTextPGM(PSTR(MSG_BP_CALIBR_ST7));

		_bLeft.setTextPGM(PSTR(MSG_CANCEL));
		_bRight.setTextPGM(PSTR(MSG_NEXT));

		_bLeft.attachPush(StateWizard::BuildPlateCancel);
		_bRight.attachPush(StateWizard::BuildPlateS8);

		DrawUpdate();

	}
}

void StateWizardZ::CompZOffsetS2(void* ptr) {
	if (!planner.movesplanned())
	{
		//Going to center adjust position
		mechanics.do_blocking_move_to_xy(int(BED_CENTER_X), int(BED_CENTER_Y), NOZZLE_PARK_XY_FEEDRATE);

		//Going to Z adjust position
		mechanics.do_blocking_move_to_z(10, mechanics.homing_feedrate_mm_s[Z_AXIS]);

		NextionHMI::ActivateState(PAGE_WIZARDZ);

		_page.show();
		NextionHMI::headerText.setTextPGM(PSTR(MSG_HEADER_COMP_Z_OFFSET));
		NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);
		_txtHeader.setTextPGM(PSTR(MSG_HEADER_COMP_Z_OFFSET ": 2/2"));
		_txtCaption.setTextPGM(PSTR(MSG_Z_OFFSET_ST2));

		_bLeft.setTextPGM(PSTR(MSG_CANCEL));
		_bRight.setTextPGM(PSTR(MSG_FINISH));

		_bLeft.attachPush(StateWizard::CompZOffsetCancel);
		_bRight.attachPush(StateWizard::CompZOffsetFinish);

		DrawUpdate();

	}
}

#endif


