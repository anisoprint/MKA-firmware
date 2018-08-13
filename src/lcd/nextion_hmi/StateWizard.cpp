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

const float filament_change_unload_length[DRIVER_EXTRUDERS] = PAUSE_PARK_UNLOAD_LENGTH,
        	filament_change_load_length[DRIVER_EXTRUDERS] = PAUSE_PARK_LOAD_LENGTH;

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

	NexTouchEventCb DrawUpdateCallback = 0;

	bool _loopStopped = false;
	bool _wizardCancelled = false;

#if HAS_BUZZER

  static void filament_change_beep(const int8_t max_beep_count, const bool init=false) {
    static millis_t next_buzz = 0;
    static int8_t runout_beep = 0;

    if (init) next_buzz = runout_beep = 0;

    const millis_t ms = millis();
    if (ELAPSED(ms, next_buzz)) {
      if (max_beep_count < 0 || runout_beep < max_beep_count + 5) { // Only beep as long as we're supposed to
        next_buzz = ms + ((max_beep_count < 0 || runout_beep < max_beep_count) ? 1000 : 500);
        BUZZ(50, 880 - (runout_beep & 1) * 220);
        runout_beep++;
      }
    }
  }

#endif

}

void StateWizard::TouchUpdate() {
	nexLoop(_listenList);
}

void StateWizard::ZAxisS1(void* ptr) {
	_wizardCancelled = false;
	//Homing
	commands.enqueue_and_echo_P(PSTR("T0"));
	commands.enqueue_and_echo_P(PSTR("G28 Z"));
	commands.enqueue_and_echo_P(PSTR("G28 X Y"));

	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_Z_OFFSET_ST1)
	HEADER(MSG_HEADER_Z_OFFSET, "1/2", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), ZAxisCancel, PSTR(MSG_NEXT), StateWizardZ::ZOffsetS2);
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



inline void StateWizard::Init1Button(const char* txtCenter, NexTouchEventCb cbCenter) {
	_bC.setTextPGM(txtCenter);
	_bC.attachPush(cbCenter);
}

inline void StateWizard::Init2Buttons(const char* txtLeft, NexTouchEventCb cbLeft,
		const char* txtRight, NexTouchEventCb cbRight) {

	_bL.setTextPGM(txtLeft);
	_bR.setTextPGM(txtRight);

	_bL.attachPush(cbLeft);
	_bR.attachPush(cbRight);
}

void StateWizard::MaterialLoadS1(void* ptr) {
	_wizardCancelled = false;
	//Homing
	commands.enqueue_and_echo_P(PSTR("T0"));
	commands.enqueue_and_echo_P(PSTR("G28 Z"));
	commands.enqueue_and_echo_P(PSTR("G28 X Y"));

	sprintf_P(NextionHMI::buffer, PSTR("G1 F%i X%i Y%i"), (int)(mechanics.homing_feedrate_mm_s[X_AXIS]*60), int(MATERIAL_CHANGE_X), int(MATERIAL_CHANGE_Y));
	commands.enqueue_and_echo(NextionHMI::buffer);

	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST1)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "1/6", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), MaterialLoadCancel, PSTR(MSG_NEXT), MaterialLoadS1a);
}

void StateWizard::MaterialLoadS1a(void* ptr) {

	uint8_t heater;
	if (NextionHMI::wizardData==E_AXIS)
	{
		heater = HOT0_INDEX;
	}
	else
	{
		if (NextionHMI::wizardData==U_AXIS || NextionHMI::wizardData==V_AXIS) heater = HOT1_INDEX;
	}
	StateTemperature::Activate(NextionHMI::autoPreheatTempHotend, heater, MaterialLoadS2, MaterialLoadCancel);

}

void StateWizard::MaterialLoadS2(void* ptr) {
	BUTTONS(1)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST2)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "2/6", NEX_ICON_MAINTENANCE);

	Init1Button(PSTR(MSG_CANCEL), MaterialLoadCancel);


	uint8_t heater;
	if (NextionHMI::wizardData==E_AXIS)
	{
		heater = HOT0_INDEX;
	}
	else
	{
		if (NextionHMI::wizardData==U_AXIS || NextionHMI::wizardData==V_AXIS) heater = HOT1_INDEX;
	}

	//to update temperature value
	DrawUpdateCallback = MaterialLoadS2DrawUpdate;

	//waiting for heating
    printer.setWaitForHeatUp(true);
    while (printer.isWaitForHeatUp() && heaters[heater].wait_for_heating()) printer.idle();
    printer.setWaitForHeatUp(false);

    DrawUpdateCallback = NULL;

    if (thermalManager.tooColdToExtrude(heater))
    	MaterialLoadS2a();
    else
    {
    	MaterialLoadS3();
    }

}

void StateWizard::MaterialLoadS2a(void* ptr) {
	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST2A)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "2/6", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), MaterialLoadCancel, PSTR(MSG_NEXT), MaterialLoadS1a);
}

void StateWizard::MaterialLoadS2DrawUpdate(void *ptr) {
	uint8_t heater;
	if (NextionHMI::wizardData==E_AXIS)
	{
		heater = HOT0_INDEX;
	}
	else
	{
		if (NextionHMI::wizardData==U_AXIS || NextionHMI::wizardData==V_AXIS) heater = HOT1_INDEX;
	}
	ZERO(NextionHMI::buffer);
	sprintf_P(NextionHMI::buffer, PSTR("%s (%d/%d\370C)"), MSG_LOAD_MATERIAL_ST2, (int)heaters[heater].current_temperature, (int)heaters[heater].target_temperature);
	Serial.println(NextionHMI::buffer);
	_text.setText(NextionHMI::buffer);
}

void StateWizard::MaterialLoadS3(void* ptr) {

	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST3)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "3/6", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), MaterialLoadCancel, PSTR(MSG_NEXT), MaterialLoadS3a);


8uuhh

    //LOOP_HOTEND()
    //  heaters[h].start_idle_timer(nozzle_timeout);

	//waiting for 90 seconds
	int max_extrude_length = (1.5*PAUSE_PARK_EXTRUDE_FEEDRATE);

	_loopStopped = false;
	int extrude_length = 0;
	while(!_loopStopped && !_wizardCancelled && extrude_length<max_extrude_length)
	{
		if (planner.movesplanned() < 2)
		{
			extrude_length += 0.5;
			mechanics.current_position[NextionHMI::wizardData] += 0.5;
			planner.buffer_line(mechanics.current_position, PAUSE_PARK_EXTRUDE_FEEDRATE, tools.active_extruder);
		}
		printer.idle();
		printer.keepalive(InProcess);
	}

	//timeout
	if (extrude_length>max_extrude_length)
	{
		_wizardCancelled = true;
		int heater;
		if (NextionHMI::wizardData==E_AXIS)
		{
			heater = HOT0_INDEX;
		}
		else
		{
			if (NextionHMI::wizardData==U_AXIS || NextionHMI::wizardData==V_AXIS) heater = HOT1_INDEX;
		}
		heaters[heater].setTarget(0);
		//StateMessage::ActivatePGM(MESSAGE_WARNING, NEX_ICON_WARNING, MSG_HEADER_LOAD_MATERIAL, message_P, numButtons, txtButtonRight_P, cbRight, txtButtonLeft_P, cbLeft, picture)

	}
	//if ok - proceed to the next step
	if (!_wizardCancelled) MaterialLoadS4();
}

void StateWizard::MaterialLoadS3a(void* ptr) {
	//Stop extruding
	_loopStopped = true;
}

void StateWizard::MaterialLoadS4(void* ptr) {
	BUTTONS(1)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST4)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "4/6", NEX_ICON_MAINTENANCE);

	Init1Button(PSTR(MSG_CANCEL), MaterialLoadCancel);

	int extrude_length = 0;
	while (extrude_length<filament_change_load_length[NextionHMI::wizardData-E_AXIS] && !_wizardCancelled)
	{
		if (planner.movesplanned() < 2)
		{
			mechanics.current_position[NextionHMI::wizardData] += 1.0;
			extrude_length += 1.0;
			planner.buffer_line(mechanics.current_position, PAUSE_PARK_LOAD_FEEDRATE, tools.active_extruder);
			Serial.println("planning");
		}
		printer.idle();
		printer.keepalive(InProcess);
	}

	Serial.println("After loop");
	if (!_wizardCancelled) MaterialLoadS5();
}

void StateWizard::MaterialLoadS5(void* ptr) {
	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST5)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "5/6", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), MaterialLoadCancel, PSTR(MSG_NEXT), MaterialLoadS5a);

	_loopStopped = false;
	while(!_loopStopped && !_wizardCancelled)
	{
		if (planner.movesplanned() < 2)
		{
			mechanics.current_position[NextionHMI::wizardData] += 0.5;
			planner.buffer_line(mechanics.current_position, PAUSE_PARK_EXTRUDE_FEEDRATE, tools.active_extruder);
		}
		printer.idle();
		printer.keepalive(InProcess);
	}
	if (!_wizardCancelled) MaterialLoadS6();
}

void StateWizard::MaterialLoadS5a(void* ptr) {
	_loopStopped = true;
}

void StateWizard::MaterialLoadS6(void* ptr) {
	if (NextionHMI::wizardData==U_AXIS)
	{
		stepper.synchronize();
		commands.enqueue_and_echo_P(PSTR("M280 P0 S160 "));
		commands.enqueue_and_echo_P(PSTR("G4 P100  "));
		commands.enqueue_and_echo_P(PSTR("M280 P0 S90 "));
	}
	BUTTONS(1)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST6)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "6/6", NEX_ICON_MAINTENANCE);

	Init1Button(PSTR(MSG_FINISH), MaterialLoadFinish);

}

void StateWizard::MaterialLoadFinish(void* ptr) {
	uint8_t heater;
	if (NextionHMI::wizardData==E_AXIS)
	{
		heater = HOT0_INDEX;
	}
	else
	{
		if (NextionHMI::wizardData==U_AXIS || NextionHMI::wizardData==V_AXIS) heater = HOT1_INDEX;
	}
	heaters[heater].setTarget(0);
	commands.enqueue_and_echo_P(PSTR("G28 Z"));
	commands.enqueue_and_echo_P(PSTR("G28 X Y"));
	StateStatus::Activate();
}

void StateWizard::MaterialLoadCancel(void* ptr) {
	_wizardCancelled = true;
	uint8_t heater;
	if (NextionHMI::wizardData==E_AXIS)
	{
		heater = HOT0_INDEX;
	}
	else
	{
		if (NextionHMI::wizardData==U_AXIS || NextionHMI::wizardData==V_AXIS) heater = HOT1_INDEX;
	}
	heaters[heater].setTarget(0);
	commands.enqueue_and_echo_P(PSTR("G28 Z"));
	commands.enqueue_and_echo_P(PSTR("G28 X Y"));
	StateStatus::Activate();
}


void StateWizard::BuildPlateS1(void* ptr) {
	_wizardCancelled = false;
	//Homing
	commands.enqueue_and_echo_P(PSTR("T0"));
	commands.enqueue_and_echo_P(PSTR("G28 Z"));
	commands.enqueue_and_echo_P(PSTR("G28 X Y"));

	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_BP_CALIBR_ST1)
	HEADER(MSG_HEADER_BP_CALIBR, "1/8", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), BuildPlateCancel, PSTR(MSG_NEXT), StateWizardZ::BuildPlateS2);
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

void StateWizard::BuildPlateS3(void* ptr) {
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
		CAPTION(MSG_BP_CALIBR_ST3)
		HEADER(MSG_HEADER_BP_CALIBR, "3/8", NEX_ICON_MAINTENANCE);

		Init2Buttons(PSTR(MSG_CANCEL), BuildPlateCancel, PSTR(MSG_NEXT), StateWizard::BuildPlateS4);
	}
}

void StateWizard::BuildPlateS4(void* ptr) {
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
		CAPTION(MSG_BP_CALIBR_ST4)
		HEADER(MSG_HEADER_BP_CALIBR, "4/8", NEX_ICON_MAINTENANCE);

		Init2Buttons(PSTR(MSG_CANCEL), BuildPlateCancel, PSTR(MSG_NEXT), StateWizard::BuildPlateS5);
	}
}

void StateWizard::BuildPlateS5(void* ptr) {
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
		CAPTION(MSG_BP_CALIBR_ST5)
		HEADER(MSG_HEADER_BP_CALIBR, "5/8", NEX_ICON_MAINTENANCE);

		Init2Buttons(PSTR(MSG_CANCEL), BuildPlateCancel, PSTR(MSG_NEXT), StateWizardZ::BuildPlateS6);
	}
}

void StateWizard::BuildPlateS7(void* ptr) {
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
		CAPTION(MSG_BP_CALIBR_ST7)
		HEADER(MSG_HEADER_BP_CALIBR, "7/8", NEX_ICON_MAINTENANCE);

		Init2Buttons(PSTR(MSG_CANCEL), BuildPlateCancel, PSTR(MSG_NEXT), StateWizard::BuildPlateS8);
	}
}

void StateWizard::BuildPlateS8(void* ptr) {
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
		CAPTION(MSG_BP_CALIBR_ST8)
		HEADER(MSG_HEADER_BP_CALIBR, "8/8", NEX_ICON_MAINTENANCE);

		Init2Buttons(PSTR(MSG_REPEAT), StateWizardZ::BuildPlateS6, PSTR(MSG_FINISH), StateWizard::BuildPlateFinish);
	}
}

void StateWizard::DrawUpdate() {
	if (DrawUpdateCallback!=NULL)
	{

		DrawUpdateCallback(NULL);
	}
}



#endif

