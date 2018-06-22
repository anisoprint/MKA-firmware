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

	//Maintenance menu
	void ActivateMaintenance(void *ptr=0);
	void Maintenance_Move(void *ptr);
	void Maintenance_Calibrate(void *ptr);
	void MaintenanceBack(void *ptr);

	//Level Build plate menu
	void ActivateCalibrate(void *ptr=0);
	void Calibrate_Buildplate(void *ptr);
	void Calibrate_ZOffset(void *ptr);
	void CalibrateBack(void *ptr);

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEMENU_H_ */
