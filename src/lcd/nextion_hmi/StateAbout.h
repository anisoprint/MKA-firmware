/*
 * StateAbout.h
 *
 *  Created on: 5 ???. 2018 ?.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEABOUT_H_
#define SRC_LCD_NEXTION_HMI_STATEABOUT_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateAbout {

	void Back_Push(void *ptr);

	void Init();
	void Activate();
	void TouchUpdate();

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEABOUT_H_ */
