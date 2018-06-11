/*
 * StateMovement.h
 *
 *  Created on: 10 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEMOVEMENT_H_
#define SRC_LCD_NEXTION_HMI_STATEMOVEMENT_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateMovement {

	void Extruders_Push(void *ptr);
	void Back_Push(void *ptr);

  	void Movement_Push(void *ptr);

	void Init();
	void Activate();
	void DrawUpdate();
	void TouchUpdate();

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEMOVEMENT_H_ */
