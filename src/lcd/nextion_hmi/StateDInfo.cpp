/*
 * StateDInfo.cpp
 *
 *  Created on: 25 сент. 2019 г.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_DINFO,  0,  "dinfo");

	NexObject _tT  = NexObject(PAGE_DINFO,  6,  "tT");

	//Buttons
	NexObject _bBack = NexObject(PAGE_DINFO,  5,  "bB");

	NexObject *_listenList[] = { &_bBack, NULL };

	NexTouchEventCb cbSetReturn = 0;
}

void StateDInfo::Back_Push(void* ptr) {
	StateMenu::ActivateMaintenance();
}

void StateDInfo::Init() {
}

void StateDInfo::Activate(NexTouchEventCb cbBack) {
	_bBack.attachPush(cbBack);

	NextionHMI::ActivateState(PAGE_DINFO);
	_page.show();

	ZERO(NextionHMI::buffer);
	sprintf_P(NextionHMI::buffer, PSTR("%s %s %s"), CUSTOM_MACHINE_NAME, eeprom.printerVersion, eeprom.printerSN);
	NextionHMI::headerText.setText(NextionHMI::buffer);

	DrawUpdate();
}

void StateDInfo::TouchUpdate() {
	nexLoop(_listenList);
}

void StateDInfo::DrawUpdate() {
	ZERO(NextionHMI::buffer);

	bool Xe = READ(X_MIN_PIN)^endstops.isLogic(X_MIN);
	bool Ye = READ(Y_MIN_PIN)^endstops.isLogic(Y_MIN);
	bool Ze = READ(Z_MAX_PIN)^endstops.isLogic(Z_MAX);

	sprintf_P(NextionHMI::buffer, PSTR(MSG_DEBUG_INFO), \
			LOGICAL_X_POSITION(mechanics.current_position[X_AXIS]), LOGICAL_Y_POSITION(mechanics.current_position[Y_AXIS]), LOGICAL_Z_POSITION(mechanics.current_position[Z_AXIS]), \
			mechanics.current_position[E_AXIS], mechanics.current_position[U_AXIS], mechanics.current_position[V_AXIS], tools.active_extruder, \
			heaters[HOT0_INDEX].current_temperature, heaters[HOT0_INDEX].target_temperature, heaters[HOT1_INDEX].current_temperature, heaters[HOT1_INDEX].target_temperature, \
			heaters[BED_INDEX].current_temperature, heaters[BED_INDEX].target_temperature, heaters[CHAMBER_INDEX].current_temperature, \
			fans[0].Speed, fans[1].Speed, fans[2].Speed, \
			Xe, Ye, Ze);

	_tT.setText(NextionHMI::buffer);
}

#endif

