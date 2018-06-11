/*
 * StateTemperature.cpp
 *
 *  Created on: 6 ���. 2018 �.
 *      Author: Azarov
 */

#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateTemperature.h"


namespace {

	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_TEMPERATURE,  0,  "temperature");

	//Variables
	NexObject _vMaxTemp=NexObject(PAGE_TEMPERATURE,  21,  "$maxTemp");
	NexObject _vAutoTemp=NexObject(PAGE_TEMPERATURE,  23,  "$autoTemp");
	NexObject _vTargetTemp=NexObject(PAGE_TEMPERATURE,  17,  "$targetTemp");

	//Current Temperature
	NexObject _tCurrentTemperatureValue= NexObject(PAGE_TEMPERATURE,  9,  "tCTvalue");
	//Target Temperature
	NexObject _tTargetTemperatureValue= NexObject(PAGE_TEMPERATURE,  11,  "tTTvalue");
	//Buttons
	NexObject _bSet = NexObject(PAGE_TEMPERATURE,  6,  "bSet");
	NexObject _bCancel = NexObject(PAGE_TEMPERATURE,  7,  "bCancel");

	NexObject *_listenList[] = {&_bSet, &_bCancel, NULL};
}


void StateTemperature::Set_Push(void* ptr) {
	uint32_t targetTemp=0;
	targetTemp = _vTargetTemp.getValue();
	if (targetTemp<=21) targetTemp=0;
	heaters[NextionHMI::pageData].setTarget(targetTemp);
	StateStatus::Activate();
}

void StateTemperature::Cancel_Push(void* ptr) {
	StateStatus::Activate();
}

void StateTemperature::Init() {
	_bSet.attachPush(Set_Push);
	_bCancel.attachPush(Cancel_Push);
}

void StateTemperature::Activate(uint16_t autoTemp, uint8_t heater) {
	NextionHMI::ActivateState(PAGE_TEMPERATURE);
	_page.show();

	NextionHMI::pageData = heater;

	_vMaxTemp.setValue(heaters[heater].maxtemp);

	int16_t target_temp = heaters[heater].target_temperature;
	if (target_temp<20) target_temp=20;

#if HAS_HEATER_BED
	if (heater == BED_INDEX)
	{
		NextionHMI::headerText.setTextPGM(PSTR(MSG_BUILDPLATE_TEMP));
	}
	else
	{
		if (heater == HOT0_INDEX) NextionHMI::headerText.setTextPGM(PSTR(MSG_PLASTIC_EXTRUDER_TEMP));
		if (heater == HOT1_INDEX) NextionHMI::headerText.setTextPGM(PSTR(MSG_COMPOSITE_EXTRUDER_TEMP));
	}
#else
	if (heater == HOT0_INDEX) NextionHMI::headerText.setTextPGM(PSTR(PLASTIC_EXTRUDER_TEMP));
	if (heater == HOT1_INDEX) NextionHMI::headerText.setTextPGM(PSTR(COMPOSITE_EXTRUDER_TEMP));
#endif

	_vAutoTemp.setValue(autoTemp);

	String str = String(target_temp) + "\370C";
	_tTargetTemperatureValue.setText(str.c_str());
	_vTargetTemp.setValue(target_temp);

	DrawUpdate();
}

void StateTemperature::DrawUpdate() {
	String str = String(round(heaters[NextionHMI::pageData].current_temperature)) + "\370C";
	_tCurrentTemperatureValue.setText(str.c_str());
}

void StateTemperature::TouchUpdate() {
	nexLoop(_listenList);
}

#endif
