/*
 * StateMenu.h
 *
 *  Created on: 10 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEMENU_H_
#define SRC_LCD_NEXTION_HMI_STATEMENU_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateMenu {

	void TouchUpdate();

	//Print control menu
	void ActivatePrintControl(void *ptr=0);
	void PrintControlBack(void *ptr);

	//Maintenance menu
	void ActivateMaintenance(void *ptr=0);
	void Maintenance_Move(void *ptr);
	void MaintenanceBack(void *ptr);

	//Calibrate menu
	void ActivateCalibrate(void *ptr=0);

	//Materials menu
	void ActivateMaterials(void *ptr=0);
	void Materials_Plastic(void *ptr=0);
	void Materials_CompPlastic(void *ptr=0);
	void Materials_CompFiber(void *ptr=0);
	void ActivateLoadUnload(void *ptr=0);

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEMENU_H_ */
