/*
 * StateDInfo.h
 *
 *  Created on: 25 сент. 2019 г.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEDINFO_H_
#define SRC_LCD_NEXTION_HMI_STATEDINFO_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateDInfo {

	void Back_Push(void *ptr);

	void Init();
	void Activate(NexTouchEventCb cbBack);
	void TouchUpdate();
	void DrawUpdate();

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEDINFO_H_ */
