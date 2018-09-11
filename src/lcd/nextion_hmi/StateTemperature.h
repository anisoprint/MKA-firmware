/*
 * StateTemperature.h
 *
 *  Created on: 6 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATETEMPERATURE_H_
#define SRC_LCD_NEXTION_HMI_STATETEMPERATURE_H_

#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateTemperature {

  	void Set_Push(void *ptr);
  	void Cancel_Push(void *ptr);

	void Activate(uint8_t hotend);
	void Activate(uint8_t heater, NexTouchEventCb cbOK, NexTouchEventCb cbCancel);
	void DrawUpdate();
	void TouchUpdate();

};

#endif




#endif /* SRC_LCD_NEXTION_HMI_STATETEMPERATURE_H_ */
