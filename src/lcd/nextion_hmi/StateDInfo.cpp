/*
 * StateDInfo.cpp
 *
 *  Created on: 25 ����. 2019 �.
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
	DrawUpdate();
}

void StateDInfo::TouchUpdate() {
	nexLoop(_listenList);
}

void StateDInfo::DrawUpdate() {
	ZERO(NextionHMI::buffer);

	sprintf_P(NextionHMI::buffer, PSTR(MSG_DEBUG_INFO), \
			LOGICAL_X_POSITION(mechanics.current_position[X_AXIS]), LOGICAL_Y_POSITION(mechanics.current_position[Y_AXIS]), LOGICAL_Z_POSITION(mechanics.current_position[Z_AXIS]), \
			mechanics.current_position[E_AXIS], mechanics.current_position[U_AXIS], mechanics.current_position[V_AXIS], \
			heaters[HOT0_INDEX].current_temperature, heaters[HOT0_INDEX].target_temperature, heaters[HOT1_INDEX].current_temperature, heaters[HOT1_INDEX].target_temperature, \
			heaters[BED_INDEX].current_temperature, heaters[BED_INDEX].target_temperature, heaters[CHAMBER_INDEX].current_temperature, \
			fans[0].Speed, fans[1].Speed, fans[2].Speed);

	_tT.setText(NextionHMI::buffer);
}

#endif

