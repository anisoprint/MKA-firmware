/*
 * StateWizard.h
 *
 *  Created on: 26 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEWIZARD_H_
#define SRC_LCD_NEXTION_HMI_STATEWIZARD_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"

#define BED_CENTER_ADJUST_X (X_MAX_POS/2)
#define BED_CENTER_ADJUST_Y (Y_MAX_POS - 15)
#define BED_LEFT_ADJUST_X 10
#define BED_LEFT_ADJUST_Y 20
#define BED_RIGHT_ADJUST_X (X_MAX_POS - 10)
#define BED_RIGHT_ADJUST_Y 20

#define LEVELING_OFFSET 0.1


namespace StateWizard {

	void TouchUpdate();

	//Z offset calibration
	void ZOffsetS0(void *ptr=0);
	void ZOffsetFinish(void *ptr=0);
	void ZOffsetCancel(void *ptr=0);

	void Init2Buttons(const char *txtLeft, NexTouchEventCb cbLeft, const char *txtRight, NexTouchEventCb cbRight);
	void Init1Button(const char *txtCenter, NexTouchEventCb cbCenter);



	//Maintenance menu
	//void ActivateMaintenance(void *ptr=0);
	//void Maintenance_Move(void *ptr);
	//void Maintenance_Calibrate(void *ptr);
	//void MaintenanceBack(void *ptr);

	//Level Build plate menu
	//void ActivateCalibrate(void *ptr=0);
	//void Calibrate_Buildplate(void *ptr);
	//void Calibrate_ZOffset(void *ptr);
	//void CalibrateBack(void *ptr);

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEWIZARD_H_ */
