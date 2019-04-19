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
#define BED_LEFT_ADJUST_X 25
#define BED_LEFT_ADJUST_Y 20
#define BED_RIGHT_ADJUST_X (X_MAX_POS - 10)
#define BED_RIGHT_ADJUST_Y 20

#define BED_CENTER_X (X_MAX_POS/2)
#define BED_CENTER_Y (Y_MAX_POS/2)

#define MATERIAL_CHANGE_X (X_MAX_POS/2)
#define MATERIAL_CHANGE_Y (Y_MAX_POS/10)

#define LEVELING_OFFSET 0.1

namespace StateWizard {

	void TouchUpdate();
	void DrawUpdate();

	//Build plate calibration
	void BuildPlateS1(void *ptr=0);
	void BuildPlateS2(void *ptr=0);
	void BuildPlateS4(void *ptr=0);
	void BuildPlateS5(void *ptr=0);
	void BuildPlateS6(void *ptr=0);
	void BuildPlateS8(void *ptr=0);
	void BuildPlateS9(void *ptr=0);
	void BuildPlateFinish(void *ptr=0);
	void BuildPlateCancel(void *ptr=0);

	//Material unload
	void MaterialUnloadS1(void *ptr=0);
	void MaterialUnloadS1a(void *ptr=0);
	void MaterialUnloadS2(void *ptr=0);
	void MaterialUnloadS2DrawUpdate(void *ptr=0);
	void MaterialUnloadS2a(void *ptr=0);
	void MaterialUnloadS3(void *ptr=0);
	void MaterialUnloadS4(void *ptr=0);
	void MaterialUnloadFinish(void *ptr=0);
	void MaterialUnloadCancel(void *ptr=0);

	//Material load
	void MaterialLoadS1(void *ptr=0);
	void MaterialLoadS1a(void *ptr=0);
	void MaterialLoadS2(void *ptr=0);
	void MaterialLoadS2DrawUpdate(void *ptr=0);
	void MaterialLoadS2a(void *ptr=0);
	void MaterialLoadS3(void *ptr=0);
	void MaterialLoadS3a(void *ptr=0);
	void MaterialLoadS4(void *ptr=0);
	void MaterialLoadS5(void *ptr=0);
	void MaterialLoadS5a(void *ptr=0);
	void MaterialLoadS6(void *ptr=0);
	void MaterialLoadFinish(void *ptr=0);
	void MaterialLoadCancel(void *ptr=0);

	//Z offset calibration
	void ZAxisS1(void *ptr=0);
	void ZAxisFinish(void *ptr=0);
	void ZAxisCancel(void *ptr=0);

	//Z offset calibration
	void CompZOffsetS1(void *ptr=0);
	void CompZOffsetFinish(void *ptr=0);
	void CompZOffsetCancel(void *ptr=0);

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
