/*
 * NextionHMI.h
 *
 *  Created on: 31 мая 2018 г.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_NEXTIONHMI_H_
#define SRC_LCD_NEXTION_HMI_NEXTIONHMI_H_

#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"

namespace NextionHMI {

	extern NexObject headerText;
	extern NexObject headerIcon;
	extern NexObject sdText;
	extern NexObject sdIcon;

	void Init();
	void DrawUpdate();
	void TouchUpdate();
	void ActivateState(uint8_t state_id);
};


#endif

#endif /* SRC_LCD_NEXTION_HMI_NEXTIONHMI_H_ */
