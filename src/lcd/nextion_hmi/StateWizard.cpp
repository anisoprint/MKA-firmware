/*
 * StateWizard.cpp
 *
 *  Created on: 26 θών. 2018 γ.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#define BUTTONS(n) 					NextionHMI::ActivateState(PAGE_WIZARD);_buttonsNum.setValue(n);
#define NO_PICTURE 					_pic.setValue(0);

#define CAPTION(n) 					_text.setTextPGM(PSTR(n));
#define HEADER(str, stage, icon) 	_head.setTextPGM(PSTR(str ": " stage));_page.show(); NextionHMI::headerText.setTextPGM(PSTR(str)); NextionHMI::headerIcon.setPic(icon);

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
	NexObject _bL = NexObject(PAGE_WIZARD,  10,   "bL");
	NexObject _bR = NexObject(PAGE_WIZARD,  2,  "bR");
	NexObject _bC = NexObject(PAGE_WIZARD,  16,   "bC");

	NexObject *_listenList[] = { &_bL, &_bR, &_bC, NULL };


}

void StateWizard::TouchUpdate() {
	nexLoop(_listenList);
}

void StateWizard::ZAxisS0(void* ptr) {
	//Homing
	commands.enqueue_and_echo_P(PSTR("T0"));
	commands.enqueue_and_echo_P(PSTR("G28 Z"));
	commands.enqueue_and_echo_P(PSTR("G28 X Y"));

	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_Z_OFFSET_ST0)
	HEADER(MSG_HEADER_Z_OFFSET, "1/2", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), ZAxisCancel, PSTR(MSG_NEXT), StateWizardZ::ZOffsetS1);
}

void StateWizard::ZAxisFinish(void* ptr) {
	if (!planner.movesplanned())
	{
		float dz = mechanics.current_position[Z_AXIS]-LEVELING_OFFSET;
		mechanics.set_home_offset(Z_AXIS, mechanics.home_offset[Z_AXIS]-dz);
		commands.enqueue_and_echo_P(PSTR("G28 Z"));
		commands.enqueue_and_echo_P(PSTR("G28 X Y"));
		eeprom.Store_Settings();
		StateStatus::Activate();
	}

}

void StateWizard::ZAxisCancel(void* ptr) {
	commands.enqueue_and_echo_P(PSTR("G28 Z"));
	commands.enqueue_and_echo_P(PSTR("G28 X Y"));
	StateStatus::Activate();
}

inline void StateWizard::Init2Buttons(const char* txtLeft, NexTouchEventCb cbLeft,
		const char* txtRight, NexTouchEventCb cbRight) {

	_bL.setTextPGM(txtLeft);
	_bR.setTextPGM(txtRight);

	_bL.attachPush(cbLeft);
	_bR.attachPush(cbRight);
}

inline void StateWizard::Init1Button(const char* txtCenter, NexTouchEventCb cbCenter) {
	_bC.setTextPGM(txtCenter);
	_bC.attachPush(cbCenter);
}

void StateWizard::BuildPlateS0(void* ptr) {
	//Homing
	commands.enqueue_and_echo_P(PSTR("T0"));
	commands.enqueue_and_echo_P(PSTR("G28 Z"));
	commands.enqueue_and_echo_P(PSTR("G28 X Y"));

	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_BP_CALIBR_ST0)
	HEADER(MSG_HEADER_BP_CALIBR, "1/8", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), BuildPlateCancel, PSTR(MSG_NEXT), StateWizardZ::BuildPlateS1);
}

void StateWizard::BuildPlateFinish(void* ptr) {
	if (!planner.movesplanned())
	{
		float dz = mechanics.current_position[Z_AXIS]-LEVELING_OFFSET;
		mechanics.set_home_offset(Z_AXIS, mechanics.home_offset[Z_AXIS]-dz);
		commands.enqueue_and_echo_P(PSTR("G28 Z"));
		commands.enqueue_and_echo_P(PSTR("G28 X Y"));
		eeprom.Store_Settings();
		StateStatus::Activate();
	}
}

void StateWizard::BuildPlateCancel(void* ptr) {
	commands.enqueue_and_echo_P(PSTR("G28 Z"));
	commands.enqueue_and_echo_P(PSTR("G28 X Y"));
	StateStatus::Activate();
}

void StateWizard::BuildPlateS2(void* ptr) {
	if (!planner.movesplanned())
	{
		//Going to left position
		mechanics.current_position[Z_AXIS] += 5;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);
		mechanics.current_position[X_AXIS] = BED_LEFT_ADJUST_X;
		mechanics.current_position[Y_AXIS] = BED_LEFT_ADJUST_Y;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[X_AXIS], tools.active_extruder);
		mechanics.current_position[Z_AXIS] -= 5;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);

		BUTTONS(2)
		NO_PICTURE
		CAPTION(MSG_BP_CALIBR_ST2)
		HEADER(MSG_HEADER_BP_CALIBR, "3/8", NEX_ICON_MAINTENANCE);

		Init2Buttons(PSTR(MSG_CANCEL), BuildPlateCancel, PSTR(MSG_NEXT), StateWizard::BuildPlateS3);
	}
}

void StateWizard::BuildPlateS3(void* ptr) {
	if (!planner.movesplanned())
	{
		//Going to right position
		mechanics.current_position[Z_AXIS] += 5;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);
		mechanics.current_position[X_AXIS] = BED_RIGHT_ADJUST_X;
		mechanics.current_position[Y_AXIS] = BED_RIGHT_ADJUST_Y;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[X_AXIS], tools.active_extruder);
		mechanics.current_position[Z_AXIS] -= 5;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);

		BUTTONS(2)
		NO_PICTURE
		CAPTION(MSG_BP_CALIBR_ST3)
		HEADER(MSG_HEADER_BP_CALIBR, "4/8", NEX_ICON_MAINTENANCE);

		Init2Buttons(PSTR(MSG_CANCEL), BuildPlateCancel, PSTR(MSG_NEXT), StateWizard::BuildPlateS4);
	}
}

void StateWizard::BuildPlateS4(void* ptr) {
	if (!planner.movesplanned())
	{
		//Going to center position
		mechanics.current_position[Z_AXIS] += 5;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);
		mechanics.current_position[X_AXIS] = BED_CENTER_ADJUST_X;
		mechanics.current_position[Y_AXIS] = BED_CENTER_ADJUST_Y;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[X_AXIS], tools.active_extruder);
		mechanics.current_position[Z_AXIS] -= 5;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);

		BUTTONS(2)
		NO_PICTURE
		CAPTION(MSG_BP_CALIBR_ST4)
		HEADER(MSG_HEADER_BP_CALIBR, "5/8", NEX_ICON_MAINTENANCE);

		Init2Buttons(PSTR(MSG_CANCEL), BuildPlateCancel, PSTR(MSG_NEXT), StateWizardZ::BuildPlateS5);
	}
}

void StateWizard::BuildPlateS6(void* ptr) {
	if (!planner.movesplanned())
	{
		//Going to left position
		mechanics.current_position[Z_AXIS] += 5;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);
		mechanics.current_position[X_AXIS] = BED_LEFT_ADJUST_X;
		mechanics.current_position[Y_AXIS] = BED_LEFT_ADJUST_Y;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[X_AXIS], tools.active_extruder);
		mechanics.current_position[Z_AXIS] -= 5;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);

		BUTTONS(2)
		NO_PICTURE
		CAPTION(MSG_BP_CALIBR_ST6)
		HEADER(MSG_HEADER_BP_CALIBR, "7/8", NEX_ICON_MAINTENANCE);

		Init2Buttons(PSTR(MSG_CANCEL), BuildPlateCancel, PSTR(MSG_NEXT), StateWizard::BuildPlateS7);
	}
}

void StateWizard::BuildPlateS7(void* ptr) {
	if (!planner.movesplanned())
	{
		//Going to right position
		mechanics.current_position[Z_AXIS] += 5;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);
		mechanics.current_position[X_AXIS] = BED_RIGHT_ADJUST_X;
		mechanics.current_position[Y_AXIS] = BED_RIGHT_ADJUST_Y;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[X_AXIS], tools.active_extruder);
		mechanics.current_position[Z_AXIS] -= 5;
		planner.buffer_line(mechanics.current_position, mechanics.homing_feedrate_mm_s[Z_AXIS], tools.active_extruder);

		BUTTONS(2)
		NO_PICTURE
		CAPTION(MSG_BP_CALIBR_ST7)
		HEADER(MSG_HEADER_BP_CALIBR, "8/8", NEX_ICON_MAINTENANCE);

		Init2Buttons(PSTR(MSG_REPEAT), StateWizardZ::BuildPlateS5, PSTR(MSG_FINISH), StateWizard::BuildPlateFinish);
	}
}

#endif

