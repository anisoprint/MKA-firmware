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

	extern uint8_t pageState;
	extern uint8_t pageData;

	extern HMIevent lastEvent;
	extern uint8_t  lastEventArg;

	extern char buffer[70];

	void Init();
	void DrawUpdate();
	void TouchUpdate();
	void ActivateState(uint8_t state_id);

	uint8_t GetActiveState();

	void RaiseEvent(HMIevent event, uint8_t eventArg);
};


#endif

#endif /* SRC_LCD_NEXTION_HMI_NEXTIONHMI_H_ */
