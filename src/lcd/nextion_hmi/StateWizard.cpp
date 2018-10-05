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

void StateWizard::TouchUpdate() {
	nexLoop(_listenList);
}


void StateWizard::ZAxisS1(void* ptr) {
	_wizardCancelled = false;

	//Homing if not homed
	if (mechanics.axis_unhomed_error())
	{
		commands.enqueue_and_echo_P(PSTR("G28 X Y"));
	}
	commands.enqueue_and_echo_P(PSTR("G28 Z"));

	//Changing tool
	if (tools.active_extruder!=0)
	{
		commands.enqueue_and_echo_P(PSTR("T0"));
	}

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


//UNLOAD 1 - Home if needed, move to change position, display info about printhead heating
void StateWizard::MaterialUnloadS1(void* ptr) {
	_wizardCancelled = false;

	if (PrintPause::Status!=Paused)
	{
		//Homing if not homed
		if (mechanics.axis_unhomed_error())
		{
			commands.enqueue_and_echo_P(PSTR("G28 X Y"));
		}
		commands.enqueue_and_echo_P(PSTR("G28 Z"));
	}
	else
	{
		const uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
	    const millis_t nozzle_timeout = (millis_t)(PAUSE_PARK_NOZZLE_TIMEOUT) * 1000UL;
	    heaters[heater].start_idle_timer(nozzle_timeout);
	}

	//Changing tool
	const uint8_t extruder_id = Tools::extruder_driver_to_extruder(NextionHMI::wizardData - E_AXIS);
	if (extruder_id!=tools.active_extruder) tools.change(extruder_id, 0, false, false);


	// Move XY to change position
	mechanics.do_blocking_move_to_xy(int(MATERIAL_CHANGE_X), int(MATERIAL_CHANGE_Y), NOZZLE_PARK_XY_FEEDRATE);

	commands.enqueue_and_echo(NextionHMI::buffer);

	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_UNLOAD_MATERIAL_ST1)
	HEADER(MSG_HEADER_UNLOAD_MATERIAL, "1/4", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), MaterialUnloadCancel, PSTR(MSG_NEXT), MaterialUnloadS1a);
}

//UNLOAD 1a - display set temperature screen
void StateWizard::MaterialUnloadS1a(void* ptr) {
	if (!planner.movesplanned())
	{
		const uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
		StateTemperature::Activate(heater, MaterialUnloadS2, MaterialUnloadCancel);
	}
}

//UNLOAD 2 - Wait for temperature to reach target
void StateWizard::MaterialUnloadS2(void* ptr) {
	BUTTONS(1)
	NO_PICTURE
	HEADER(MSG_HEADER_UNLOAD_MATERIAL, "2/4", NEX_ICON_MAINTENANCE);
	MaterialUnloadS2DrawUpdate();
	Init1Button(PSTR(MSG_CANCEL), MaterialLoadCancel);

	const uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
    heaters[heater].reset_idle_timer();

	//to update temperature value
	DrawUpdateCallback = MaterialUnloadS2DrawUpdate;

	//Temperature::wait_heater(&heaters[heater]);

	//waiting for heating
    printer.setWaitForHeatUp(true);
    while (printer.isWaitForHeatUp() && heaters[heater].wait_for_heating()) printer.idle();
    printer.setWaitForHeatUp(false);

    DrawUpdateCallback = NULL;

    if (!_wizardCancelled)
		{
		if (thermalManager.tooColdToExtrude(heater))
			MaterialUnloadS2a();
		else
		{
			MaterialUnloadS3();
		}
    }

}

void StateWizard::MaterialUnloadS2DrawUpdate(void *ptr) {
	uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
	ZERO(NextionHMI::buffer);
	sprintf_P(NextionHMI::buffer, PSTR(MSG_UNLOAD_MATERIAL_ST2), (int)heaters[heater].current_temperature, (int)heaters[heater].target_temperature);
	_text.setText(NextionHMI::buffer);
}

//UNLOAD 2a - Display error if temperature is too low
void StateWizard::MaterialUnloadS2a(void* ptr) {
	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_UNLOAD_MATERIAL_ST2A)
	HEADER(MSG_HEADER_UNLOAD_MATERIAL, "2/4", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), MaterialUnloadCancel, PSTR(MSG_NEXT), MaterialUnloadS1a);
}


//UNLOAD 3 - Unload material
void StateWizard::MaterialUnloadS3(void* ptr) {
	BUTTONS(0)
	NO_PICTURE
	CAPTION(MSG_UNLOAD_MATERIAL_ST3)
	HEADER(MSG_HEADER_UNLOAD_MATERIAL, "3/4", NEX_ICON_MAINTENANCE);

	if (Tools::extruder_driver_is_plastic((AxisEnum)NextionHMI::wizardData)) //Unload plastic
	{
	    // Retract filament
		//PrintPause::DoPauseExtruderMove((AxisEnum)NextionHMI::wizardData, -FILAMENT_UNLOAD_RETRACT_LENGTH, PAUSE_PARK_RETRACT_FEEDRATE);
	    // Wait for filament to cool
	    //printer.safe_delay(FILAMENT_UNLOAD_DELAY);
	    // Quickly purge
	    // PrintPause::DoPauseExtruderMove((AxisEnum)NextionHMI::wizardData, FILAMENT_UNLOAD_RETRACT_LENGTH + FILAMENT_UNLOAD_PURGE_LENGTH, mechanics.max_feedrate_mm_s[(AxisEnum)NextionHMI::wizardData]);

	}

    // Unload filament
	//PrintPause::DoPauseExtruderMove((AxisEnum)NextionHMI::wizardData, -filament_change_unload_length[NextionHMI::wizardData-E_AXIS], PAUSE_PARK_UNLOAD_FEEDRATE);

    MaterialUnloadS4();
}

//UNLOAD 3 - Display finish message
void StateWizard::MaterialUnloadS4(void* ptr) {
	BUTTONS(1)
	NO_PICTURE
	CAPTION(MSG_UNLOAD_MATERIAL_ST4)
	HEADER(MSG_HEADER_UNLOAD_MATERIAL, "4/4", NEX_ICON_MAINTENANCE);

	const uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
    const millis_t nozzle_timeout = (millis_t)(PAUSE_PARK_NOZZLE_TIMEOUT) * 1000UL;
    heaters[heater].start_idle_timer(nozzle_timeout);

	Init1Button(PSTR(MSG_FINISH), MaterialUnloadFinish);
}

void StateWizard::MaterialUnloadFinish(void* ptr) {
	uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);

	if (PrintPause::Status==Paused)
	{
		// Move XY to park position
	    const point_t park_point = NOZZLE_PARK_POINT;
		Nozzle::park(3, park_point);

	    const millis_t nozzle_timeout = (millis_t)(PAUSE_PARK_NOZZLE_TIMEOUT) * 1000UL;
	    heaters[heater].start_idle_timer(nozzle_timeout);
	}
	else
	{
		heaters[heater].setTarget(0);
		commands.enqueue_and_echo_P(PSTR("G28 Z"));
		commands.enqueue_and_echo_P(PSTR("G28 X Y"));
	}

	StateMenu::ActivateLoadUnload();
}

void StateWizard::MaterialUnloadCancel(void* ptr) {
	_wizardCancelled = true;
	MaterialUnloadFinish();
}


void StateWizard::MaterialLoadS1(void* ptr) {
	_wizardCancelled = false;

	if (PrintPause::Status!=Paused)
	{
		//Homing if not homed
		if (mechanics.axis_unhomed_error())
		{
			commands.enqueue_and_echo_P(PSTR("G28 X Y"));
		}
		commands.enqueue_and_echo_P(PSTR("G28 Z"));
	}
	else
	{
		const uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
	    const millis_t nozzle_timeout = (millis_t)(PAUSE_PARK_NOZZLE_TIMEOUT) * 1000UL;
	    heaters[heater].start_idle_timer(nozzle_timeout);
	}

	//Changing tool
	uint8_t extruder_id = Tools::extruder_driver_to_extruder(NextionHMI::wizardData - E_AXIS);
	if (extruder_id!=tools.active_extruder) tools.change(extruder_id, 0, false, false);

	// Move XY to change position
	mechanics.do_blocking_move_to_xy(int(MATERIAL_CHANGE_X), int(MATERIAL_CHANGE_Y), NOZZLE_PARK_XY_FEEDRATE);

	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST1)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "1/6", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), MaterialLoadCancel, PSTR(MSG_NEXT), MaterialLoadS1a);
}

void StateWizard::MaterialLoadS1a(void* ptr) {
	if (!planner.movesplanned())
	{
		uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
		StateTemperature::Activate(heater, MaterialLoadS2, MaterialLoadCancel);
	}
}

void StateWizard::MaterialLoadS2(void* ptr) {
	BUTTONS(1)
	NO_PICTURE
	HEADER(MSG_HEADER_LOAD_MATERIAL, "2/6", NEX_ICON_MAINTENANCE);
	MaterialLoadS2DrawUpdate();

	Init1Button(PSTR(MSG_CANCEL), MaterialLoadCancel);

	uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
    heaters[heater].reset_idle_timer();

	//to update temperature value
	DrawUpdateCallback = MaterialLoadS2DrawUpdate;

	//waiting for heating
    //printer.setWaitForHeatUp(true);
    //while (printer.isWaitForHeatUp() && heaters[heater].wait_for_heating()) printer.idle();
    //printer.setWaitForHeatUp(false);
	Temperature::wait_heater(&heaters[heater]);

    DrawUpdateCallback = NULL;

    if (!_wizardCancelled)
    {
		if (thermalManager.tooColdToExtrude(heater))
			MaterialLoadS2a();
		else
		{
			MaterialLoadS3();
		}
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
	uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
	ZERO(NextionHMI::buffer);
	sprintf_P(NextionHMI::buffer, PSTR(MSG_LOAD_MATERIAL_ST2), (int)heaters[heater].current_temperature, (int)heaters[heater].target_temperature);
	_text.setText(NextionHMI::buffer);
}

void StateWizard::MaterialLoadS3(void* ptr) {

	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST3)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "3/6", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), MaterialLoadCancel, PSTR(MSG_NEXT), MaterialLoadS3a);


    //LOOP_HOTEND()
    //  heaters[h].start_idle_timer(nozzle_timeout);

	//waiting for 120 seconds
	const uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
    const millis_t nozzle_timeout = (millis_t)(PAUSE_PARK_NOZZLE_TIMEOUT) * 1000UL;
    heaters[heater].start_idle_timer(nozzle_timeout);

	//int max_extrude_length = (120*PAUSE_PARK_EXTRUDE_FEEDRATE);
	//int extrude_length = 0;

	_loopStopped = false;
	while(!_loopStopped && !_wizardCancelled && !heaters[heater].isIdle())
	{
		if (planner.movesplanned() < 2)
		{
			mechanics.current_position[NextionHMI::wizardData] += 0.5;
			planner.buffer_line(mechanics.current_position, PAUSE_PARK_EXTRUDE_FEEDRATE, tools.active_extruder);
		}
		printer.idle();
		printer.keepalive(InProcess);
	}

	//timeout
	if (heaters[heater].isIdle())
	{
		LOOP_HEATER() heaters[h].setTarget(0);
		StateMessage::ActivatePGM(MESSAGE_WARNING, NEX_ICON_WARNING, MSG_HEADER_LOAD_MATERIAL, PSTR(MSG_LOAD_MATERIAL_ST3_TIMEOUT), 2, PSTR(MSG_REPEAT), MaterialLoadS2, PSTR(MSG_CANCEL), MaterialLoadCancel, 0);
	}
	else
	{
		//if ok - proceed to the next step
		if (!_wizardCancelled) MaterialLoadS4();
	}

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

	uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
    heaters[heater].reset_idle_timer();

	int extrude_length = 0;
	while (extrude_length<filament_change_load_length[NextionHMI::wizardData-E_AXIS] && !_wizardCancelled)
	{
		if (planner.movesplanned() < 2)
		{
			mechanics.current_position[NextionHMI::wizardData] += 1.0;
			extrude_length += 1.0;
			planner.buffer_line(mechanics.current_position, PAUSE_PARK_LOAD_FEEDRATE, tools.active_extruder);
		}
		printer.idle();
		printer.keepalive(InProcess);
	}

	if (!_wizardCancelled) MaterialLoadS5();
}

void StateWizard::MaterialLoadS5(void* ptr) {
	BUTTONS(2)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST5)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "5/6", NEX_ICON_MAINTENANCE);

	Init2Buttons(PSTR(MSG_CANCEL), MaterialLoadCancel, PSTR(MSG_NEXT), MaterialLoadS5a);


	//waiting for 120 seconds
	const uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);
    const millis_t nozzle_timeout = (millis_t)(PAUSE_PARK_NOZZLE_TIMEOUT) * 1000UL;
    heaters[heater].start_idle_timer(nozzle_timeout);

	_loopStopped = false;
	while(!_loopStopped && !_wizardCancelled && !heaters[heater].isIdle())
	{
		if (planner.movesplanned() < 2)
		{
			mechanics.current_position[NextionHMI::wizardData] += 0.5;
			planner.buffer_line(mechanics.current_position, PAUSE_PARK_EXTRUDE_FEEDRATE, tools.active_extruder);
		}
		printer.idle();
		printer.keepalive(InProcess);
	}


	//timeout
	if (heaters[heater].isIdle())
	{
		_wizardCancelled = true;
		LOOP_HEATER() heaters[h].setTarget(0);
		StateMessage::ActivatePGM(MESSAGE_WARNING, NEX_ICON_WARNING, MSG_HEADER_LOAD_MATERIAL, PSTR(MSG_LOAD_MATERIAL_ST5_TIMEOUT), 1, PSTR(MSG_OK), MaterialLoadCancel, 0, 0, 0);
	}
	else
	{

		if (!_wizardCancelled) MaterialLoadS6();
	}



}

void StateWizard::MaterialLoadS5a(void* ptr) {
	_loopStopped = true;
}

void StateWizard::MaterialLoadS6(void* ptr) {
	if (!Tools::extruder_driver_is_plastic((AxisEnum)NextionHMI::wizardData))
	{
		//Cut fiber
		stepper.synchronize();
		MOVE_SERVO(0, 160);
		millis_t dwell_ms = 0;
		while (PENDING(millis(), dwell_ms)) {
		  printer.keepalive(InProcess);
		  printer.idle();
		}
		MOVE_SERVO(0, 90);
	}
	BUTTONS(1)
	NO_PICTURE
	CAPTION(MSG_LOAD_MATERIAL_ST6)
	HEADER(MSG_HEADER_LOAD_MATERIAL, "6/6", NEX_ICON_MAINTENANCE);

	Init1Button(PSTR(MSG_FINISH), MaterialLoadFinish);

}

void StateWizard::MaterialLoadFinish(void* ptr) {
	uint8_t heater = Tools::extruder_driver_to_extruder(NextionHMI::wizardData-E_AXIS);

	if (PrintPause::Status==Paused)
	{
		// Move XY to park position
	    const point_t park_point = NOZZLE_PARK_POINT;
		Nozzle::park(3, park_point);

	    const millis_t nozzle_timeout = (millis_t)(PAUSE_PARK_NOZZLE_TIMEOUT) * 1000UL;
	    heaters[heater].start_idle_timer(nozzle_timeout);
	}
	else
	{
		heaters[heater].setTarget(0);
		commands.enqueue_and_echo_P(PSTR("G28 Z"));
		commands.enqueue_and_echo_P(PSTR("G28 X Y"));
	};
	StateMenu::ActivateLoadUnload();
}

void StateWizard::MaterialLoadCancel(void* ptr) {
	_wizardCancelled = true;
	MaterialLoadFinish();
}

void StateWizard::BuildPlateS1(void* ptr) {
	_wizardCancelled = false;

	//Homing if not homed
	if (mechanics.axis_unhomed_error())
	{
		commands.enqueue_and_echo_P(PSTR("G28 X Y"));
	}
	commands.enqueue_and_echo_P(PSTR("G28 Z"));

	//Changing tool
	if (tools.active_extruder!=0)
	{
		commands.enqueue_and_echo_P(PSTR("T0"));
	}

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

		Init2Buttons(PSTR(MSG_REPEAT), StateWizard::BuildPlateS5, PSTR(MSG_FINISH), StateWizard::BuildPlateFinish);
	}
}

void StateWizard::DrawUpdate() {
	if (DrawUpdateCallback!=NULL)
	{

		DrawUpdateCallback(NULL);
	}
}



#endif

