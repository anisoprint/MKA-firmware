/*
 * StateStatus2.h
 *
 *  Created on: 1 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATESTATUS2_H_
#define SRC_LCD_NEXTION_HMI_STATESTATUS2_H_

#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateStatus {

  	void Temperature_Push(void *ptr);
  	void Print_Push(void *ptr);
  	void Maintenance_Push(void *ptr);

	void Init();
	void Activate();
	void DrawUpdate();
	void TouchUpdate();

};

#endif



#endif /* SRC_LCD_NEXTION_HMI_STATESTATUS2_H_ */
