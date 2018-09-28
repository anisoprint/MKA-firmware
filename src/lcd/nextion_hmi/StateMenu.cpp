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
 * Maintenance
 *
 *********************************************************************************/

void StateMenu::ActivateMaintenance(void* ptr) {
	NextionHMI::ActivateState(PAGE_MENU);
	_count.setValue(5);
	_page.show();
	NextionHMI::headerText.setTextPGM(PSTR(MSG_MAINTENANCE));
	NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

	_b1.setTextPGM(PSTR(MSG_MATERIALS));
	_b2.setTextPGM(PSTR(MSG_MOVE));
	_b3.setTextPGM(PSTR(MSG_CALIBRATE));
	_b4.setTextPGM(PSTR(MSG_SETTINGS));
	_b5.setTextPGM(PSTR(MSG_ABOUT_PRINTER));

	_b1.attachPush(ActivateMaterials);
	_b2.attachPush(Maintenance_Move);
	_b3.attachPush(ActivateCalibrate);
	_b4.attachPush(0);
	_b5.attachPush(0);

	_bBack.attachPush(MaintenanceBack);
}

void StateMenu::MaintenanceBack(void* ptr) {
	StateStatus::Activate();
}

void StateMenu::Maintenance_Move(void* ptr) {
	StateMovement::Activate(MODE_MOVE_AXIS);
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
	_b3.attachPush(0);

	_bBack.attachPush(ActivateMaintenance);
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
	NextionHMI::headerText.setTextPGM(PSTR(MSG_MATERIALS));
	NextionHMI::headerIcon.setPic(NEX_ICON_MAINTENANCE);

	_b1.setTextPGM(PSTR(MSG_LOAD));
	_b2.setTextPGM(PSTR(MSG_UNLOAD));

	_b1.attachPush(StateWizard::MaterialLoadS1);
	_b2.attachPush(StateWizard::MaterialUnloadS1);

	_bBack.attachPush(ActivateMaterials);
}

#endif


