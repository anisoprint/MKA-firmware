/*
 * StateStatus2.cpp
 *
 *  Created on: 1 θών. 2018 γ.
 *      Author: Azarov
 */


#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateStatus.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_STATUS,  0,  "status");
	//Build plate status
	NexObject _tBuildPlateTemperature = NexObject(PAGE_STATUS,  14,  "tBPt");
	NexObject _tBuildPlateColorStatus = NexObject(PAGE_STATUS,  11,  "tBPc");
	NexObject _bBuildPlate= NexObject(PAGE_STATUS,  19,  "bBP");
	//Print head status
	NexObject _tH0Temperature = NexObject(PAGE_STATUS,  9,  "tH0t");
	NexObject _tH0ColorStatus = NexObject(PAGE_STATUS,  5,  "tH0c");
	NexObject _bH0 = NexObject(PAGE_STATUS, 18, "bH0");
	NexObject _tH1Temperature = NexObject(PAGE_STATUS,  24,  "tH1t");
	NexObject _tH1ColorStatus = NexObject(PAGE_STATUS,  21,  "tH1c");
	NexObject _bH1 = NexObject(PAGE_STATUS,  25, "bH1");
	//Buttons
	NexObject _bPrint = NexObject(PAGE_STATUS,  15,  "bPrint");
	NexObject _bMaintenance = NexObject(PAGE_STATUS,  16,  "bMaintenance");

	NexObject *_listenList[] = {&_bH0, &_bH1, &_bBuildPlate, &_bPrint, &_bMaintenance, NULL};
};

void StateStatus::Temperature_Push(void* ptr) {
	uint8_t heater;
	if (ptr==&_bBuildPlate)
	{
		heater = BED_INDEX;
	}
	else
	{
		if (ptr==&_bH0)
		{
			heater = HOT0_INDEX;
		}
		else
		{
			if (ptr==&_bH1) heater = HOT1_INDEX;
		}
	}
	StateTemperature::Activate(heater);

}

void StateStatus::Print_Push(void* ptr) {
	StateFiles::Activate();
}

void StateStatus::Maintenance_Push(void* ptr) {
	StateMenu::ActivateMaintenance();
}

void StateStatus::Init() {
	_bH0.attachPush(Temperature_Push, &_bH0);
	_bH1.attachPush(Temperature_Push, &_bH1);
	_bBuildPlate.attachPush(Temperature_Push, &_bBuildPlate);
	_bPrint.attachPush(Print_Push);
	_bMaintenance.attachPush(Maintenance_Push);
}

void StateStatus::Activate() {
	NextionHMI::ActivateState(PAGE_STATUS);
	_page.show();
	//SERIAL_MSG("SHOW \n");
	NextionHMI::headerText.setTextPGM(PSTR(WELCOME_MSG));
	DrawUpdate();
}

void StateStatus::DrawUpdate() {
	//HOTENDS TEMPERATURE
	String strTemp;
	HeatingStatus hs;

	//H0 temperature
	hs = heaters[HOT0_INDEX].heaterStatus();
	strTemp = String(round(heaters[HOT0_INDEX].current_temperature));
	if (hs==HeatingStatus::HEATING || hs==HeatingStatus::COOLING)
	{
		strTemp += "/";
		strTemp += (heaters[HOT0_INDEX].target_temperature<20) ? 20 : heaters[HOT0_INDEX].target_temperature;
	}
	strTemp += "\370C";
	_tH0Temperature.setText(strTemp.c_str());
	//H0 color
	if (hs==HeatingStatus::COOLING || hs==HeatingStatus::COOL)
	{
		_tH0ColorStatus.Set_background_color_bco(29582);
	}
	else
	{
		if (hs==HeatingStatus::HOT) _tH0ColorStatus.Set_background_color_bco(21800);
		else _tH0ColorStatus.Set_background_color_bco(56160);
	}

#if HOTENDS>1
	//H1 temperature
	hs = heaters[HOT1_INDEX].heaterStatus();
	strTemp = String(round(heaters[HOT1_INDEX].current_temperature));
	if (hs==HeatingStatus::HEATING || hs==HeatingStatus::COOLING)
	{
		strTemp += "/";
		strTemp += (heaters[HOT1_INDEX].target_temperature<20) ? 20 : heaters[HOT1_INDEX].target_temperature;
	}
	strTemp += "\370C";
	_tH1Temperature.setText(strTemp.c_str());
	//H1 color
	if (hs==HeatingStatus::COOLING || hs==HeatingStatus::COOL)
	{
		_tH1ColorStatus.Set_background_color_bco(29582);
	}
	else
	{
		if (hs==HeatingStatus::HOT) _tH1ColorStatus.Set_background_color_bco(21800);
		else _tH1ColorStatus.Set_background_color_bco(56160);
	}
#endif

#if HAS_HEATER_BED
	//BP temperature

	hs = heaters[BED_INDEX].heaterStatus();
	strTemp = String(round(heaters[BED_INDEX].current_temperature));
	if (hs==HeatingStatus::HEATING || hs==HeatingStatus::COOLING)
	{
		strTemp += "/";
		strTemp += (heaters[BED_INDEX].target_temperature<20) ? 20 : heaters[BED_INDEX].target_temperature;
	}
	strTemp += "\370C";
	_tBuildPlateTemperature.setText(strTemp.c_str());
	//H0 color
	if (hs==HeatingStatus::COOLING || hs==HeatingStatus::COOL)
	{
		_tBuildPlateColorStatus.Set_background_color_bco(29582);
	}
	else
	{
		if (hs==HeatingStatus::HOT) _tBuildPlateColorStatus.Set_background_color_bco(21800);
		else _tBuildPlateColorStatus.Set_background_color_bco(56160);
	}
#endif


}

void StateStatus::TouchUpdate() {
	nexLoop(_listenList);
}

#endif

