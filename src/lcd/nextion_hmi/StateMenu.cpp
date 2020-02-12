/*
 * StateMenu.cpp
 *
 *  Created on: 10 θών. 2018 γ.
 *      Author: Azarov
 */

#include "../../../MK4duo.h"

#if ENABLED(NEXTION_HMI)

#include "StateMenu.h"

namespace {
	///////////// Nextion components //////////
	//Page
	NexObject _page = NexObject(PAGE_MENU,  0,  "menu");

	//Variables
	NexObject _count = NexObject(PAGE_MENU,  23,   "menu.$c");

	//Buttons
	NexObject _b1 = NexObject(PAGE_MENU,  1,   "b1");
	NexObject _b2 = NexObject(PAGE_MENU,  15,  "b2");
	NexObject _b3 = NexObject(PAGE_MENU,  4,   "b3");
	NexObject _b4 = NexObject(PAGE_MENU,  18,  "b4");
	NexObject _b5 = NexObject(PAGE_MENU,  3,   "b5");
	NexObject _b6 = NexObject(PAGE_MENU,  17,  "b6");
	NexObject _b7 = NexObject(PAGE_MENU,  2,   "b7");
	NexObject _b8 = NexObject(PAGE_MENU,  16,  "b8");

	NexObject _bBack = NexObject(PAGE_MENU,  8,  "bb");

	NexObject *_listenList[] = { &_bBack, &_b1, &_b2, &_b3, &_b4, &_b5, &_b6, &_b7, &_b8, NULL };

}


void StateMenu::TouchUpdate() {
	nexLoop(_listenList);
}

/*********************************************************************************
 *
 * PrintControl
 *
 *********************************************************************************/

void StateMenu::ActivatePrintControl(void* ptr) {

	NextionHMI::ActivateState(PAGE_MENU);

	if (PrintPause::Status == Paused) //Control menu while paused
	{
		_count.setValue(6);
		_page.show();
		NextionHMI::headerText.setTextPGM(PSTR(MSG_CONTROL));
		NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

		_b1.setTextPGM(PSTR(MSG_CANCEL_PRINT));
		_b2.setTextPGM(PSTR(MSG_MOVE));
		_b3.setTextPGM(PSTR(MSG_TEMPERATURE));
		_b4.setTextPGM(PSTR(MSG_MATERIALS));
		_b5.setTextPGM(PSTR(MSG_TUNE));
		_b6.setTextPGM(PSTR(MSG_DINFO));

		_b1.attachPush(Control_CancelPrint);
		_b2.attachPush(Control_Move);
		_b3.attachPush(ActivatePrintTemperature);
		_b4.attachPush(ActivateMaterials);
		_b5.attachPush(Control_Tune);
		_b6.attachPush(Control_DInfo);
	}
	else //Control menu while printing
	{
		_count.setValue(4);
		_page.show();
		NextionHMI::headerText.setTextPGM(PSTR(MSG_CONTROL));
		NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

		_b1.setTextPGM(PSTR(MSG_CANCEL_PRINT));
		_b2.setTextPGM(PSTR(MSG_TUNE));
		_b3.setTextPGM(PSTR(MSG_CORRECT_Z));
		_b4.setTextPGM(PSTR(MSG_DINFO));

		_b1.attachPush(Control_CancelPrint);
		_b2.attachPush(Control_Tune);
		_b3.attachPush(StateWizardZ::BabystepZ);
		_b4.attachPush(Control_DInfo);
	}


	_bBack.attachPush(PrintControlBack);
}

void StateMenu::PrintControlBack(void* ptr) {
	StatePrinting::Activate();
}

void StateMenu::Control_DInfo(void* ptr) {
	StateDInfo::Activate(StateMenu::ActivatePrintControl);
}

void StateMenu::Control_Move(void* ptr) {
	StateMovement::Activate(MODE_MOVE_AXIS, StateMenu::ActivatePrintControl);
}

void StateMenu::Control_Tune(void* ptr) {
	StateSettings::Activate(StatePrinting::TuneList, TUNE_LIST_LENGTH, Control_Tune_OK);
}

void StateMenu::Control_Tune_OK(void* ptr) {
	StateMenu::ActivatePrintControl();
	#if ENABLED(VOLUMETRIC_EXTRUSION)
	  tools.calculate_volumetric_multipliers();
	#else
	  for (uint8_t i = COUNT(tools.e_factor); i--;)
		tools.refresh_e_factor(i);
	#endif

	LOOP_HEATER()
	{
		heaters[h].updateCorrection();
	}

	LOOP_FAN()
	{
		fans[f].updateCorrection();
	}

}

void StateMenu::Control_CancelPrint(void* ptr) {
	StateMessage::ActivatePGM(MESSAGE_DIALOG, NEX_ICON_WARNING, PSTR(MSG_CANCEL_PRINTING), PSTR(MSG_CONFIRM_CANCEL_PRINTING), 2, PSTR(MSG_NO), ActivatePrintControl, PSTR(MSG_YES), Control_CancelPrint_Yes);
}

void StateMenu::Control_CancelPrint_Yes(void* ptr) {
	StatePrinting::Activate();

	#if HAS_TEMP_BED && PAUSE_PARK_PRINTER_OFF > 0
	  heaters[BED_INDEX].reset_idle_timer();
	#endif

	LOOP_HOTEND() {
	  heaters[h].reset_idle_timer();
	}
	printer.setWaitForUser(false);
	printer.setAbortSDprinting(true);
	printer.setWaitForHeatUp(false);
}

/*********************************************************************************
*
* ControlTemperature
*
*********************************************************************************/

void StateMenu::ActivatePrintTemperature(void* ptr) {
	NextionHMI::ActivateState(PAGE_MENU);
	_count.setValue(3);
	_page.show();
	NextionHMI::headerText.setTextPGM(PSTR(MSG_TEMPERATURE));
	NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

	_b1.setTextPGM(PSTR(MSG_PLASTIC ));
	_b2.setTextPGM(PSTR(MSG_COMPOSITE));
	_b3.setTextPGM(PSTR(MSG_BUILD_PLATE));

	_b1.attachPush(StateMenu::PrintTemperature_Plastic);
	_b2.attachPush(StateMenu::PrintTemperature_Composite);
	_b3.attachPush(StateMenu::PrintTemperature_Buildplate);

	_bBack.attachPush(ActivatePrintControl);
}

void StateMenu::PrintTemperature_Plastic(void* ptr) {
	StateTemperature::Activate(HOT0_INDEX, StateMenu::ActivatePrintTemperature, StateMenu::ActivatePrintTemperature);
}

void StateMenu::PrintTemperature_Composite(void* ptr) {
	StateTemperature::Activate(HOT1_INDEX, StateMenu::ActivatePrintTemperature, StateMenu::ActivatePrintTemperature);
}

void StateMenu::PrintTemperature_Buildplate(void* ptr) {
	StateTemperature::Activate(BED_INDEX, StateMenu::ActivatePrintTemperature, StateMenu::ActivatePrintTemperature);
}


/*********************************************************************************
 *
 * Maintenance
 *
 *********************************************************************************/

void StateMenu::ActivateMaintenance(void* ptr) {
	NextionHMI::ActivateState(PAGE_MENU);
	_count.setValue(6);
	_page.show();
	NextionHMI::headerText.setTextPGM(PSTR(MSG_MAINTENANCE));
	NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

	_b1.setTextPGM(PSTR(MSG_MATERIALS));
	_b2.setTextPGM(PSTR(MSG_MOVE));
	_b3.setTextPGM(PSTR(MSG_CALIBRATE));
	_b4.setTextPGM(PSTR(MSG_SETTINGS));
	_b5.setTextPGM(PSTR(MSG_DINFO));
	_b6.setTextPGM(PSTR(MSG_ABOUT_PRINTER));

	_b1.attachPush(ActivateMaterials);
	_b2.attachPush(Maintenance_Move);
	_b3.attachPush(ActivateCalibrate);
	_b4.attachPush(Maintenance_Settings);
	_b5.attachPush(Maintenance_DInfo);
	_b6.attachPush(Maintenance_About);

	_bBack.attachPush(MaintenanceBack);
}

void StateMenu::MaintenanceBack(void* ptr) {
	StateStatus::Activate();
}

void StateMenu::Maintenance_Move(void* ptr) {
	StateMovement::Activate(MODE_MOVE_AXIS, StateMenu::ActivateMaintenance);
}

void StateMenu::Maintenance_DInfo(void* ptr) {
	StateDInfo::Activate(StateMenu::ActivateMaintenance);
}

void StateMenu::Maintenance_About(void* ptr) {
	StateAbout::Activate();
}

void StateMenu::Maintenance_Settings(void* ptr) {
	StateSettings::Activate(StateSettings::SettingsList, SETTINGS_LIST_LENGTH, Maintenance_Settings_OK);
}

void StateMenu::Maintenance_Settings_OK(void* ptr) {
	StateMenu::ActivateMaintenance();
	EEPROM::Postprocess();
}



/*********************************************************************************
*
* Calibrate
*
*********************************************************************************/

void StateMenu::ActivateCalibrate(void* ptr) {
	NextionHMI::ActivateState(PAGE_MENU);
	_count.setValue(3);
	_page.show();
	NextionHMI::headerText.setTextPGM(PSTR(MSG_CALIBRATE));
	NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

	_b1.setTextPGM(PSTR(MSG_BUILD_PLATE));
	_b2.setTextPGM(PSTR(MSG_Z_AXIS));
	_b3.setTextPGM(PSTR(MSG_PRINTHEAD));

	_b1.attachPush(StateWizard::BuildPlateS1);
	_b2.attachPush(StateWizard::ZAxisS1);
	_b3.attachPush(ActivateCalibrateHead);

	_bBack.attachPush(ActivateMaintenance);
}

/*********************************************************************************
*
* Calibrate printhead
*
*********************************************************************************/

void StateMenu::ActivateCalibrateHead(void* ptr) {
	NextionHMI::ActivateState(PAGE_MENU);
	_count.setValue(1);
	_page.show();
	NextionHMI::headerText.setTextPGM(PSTR(MSG_CALIBRATE_PRINTHEAD));
	NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

	_b1.setTextPGM(PSTR(MSG_COMP_Z_OFFSET ));

	_b1.attachPush(StateWizard::CompZOffsetS1);


	_bBack.attachPush(ActivateCalibrate);
}

/*********************************************************************************
*
* Materials
*
*********************************************************************************/

void StateMenu::ActivateMaterials(void* ptr) {
	NextionHMI::ActivateState(PAGE_MENU);
	_count.setValue(3);
	_page.show();
	NextionHMI::headerText.setTextPGM(PSTR(MSG_MATERIALS));
	NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

	_b1.setTextPGM(PSTR(MSG_PLASTIC));
	_b2.setTextPGM(PSTR(MSG_COMP_PLASTIC));
	_b3.setTextPGM(PSTR(MSG_COMP_FIBER));

	_b1.attachPush(Materials_Plastic);
	_b2.attachPush(Materials_CompPlastic);
	_b3.attachPush(Materials_CompFiber);

	if (PrintPause::Status == Paused)
		_bBack.attachPush(ActivatePrintControl);
	else
		_bBack.attachPush(ActivateMaintenance);
}

void StateMenu::Materials_Plastic(void* ptr) {
	NextionHMI::wizardData = E_AXIS;
	ActivateLoadUnload();
}

void StateMenu::Materials_CompPlastic(void* ptr) {
	NextionHMI::wizardData = V_AXIS;
	ActivateLoadUnload();
}

void StateMenu::Materials_CompFiber(void* ptr) {
	NextionHMI::wizardData = U_AXIS;
	ActivateLoadUnload();
}


void StateMenu::ActivateLoadUnload(void* ptr) {
	NextionHMI::ActivateState(PAGE_MENU);
	_count.setValue(2);
	_page.show();

	switch (NextionHMI::wizardData) {
		case E_AXIS:
			NextionHMI::headerText.setTextPGM(PSTR(MSG_PLASTIC));
			break;
		case U_AXIS:
			NextionHMI::headerText.setTextPGM(PSTR(MSG_COMP_FIBER));
			break;
		case V_AXIS:
			NextionHMI::headerText.setTextPGM(PSTR(MSG_COMP_PLASTIC));
			break;
		}

	NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

	_b1.setTextPGM(PSTR(MSG_LOAD));
	_b2.setTextPGM(PSTR(MSG_UNLOAD));

	_b1.attachPush(StateWizard::MaterialLoadS1);
	_b2.attachPush(StateWizard::MaterialUnloadS1);

	_bBack.attachPush(ActivateMaterials);
}



#endif


