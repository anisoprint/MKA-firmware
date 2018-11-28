/*
 * StateMessage.h
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEEDITNUMBER_H_
#define SRC_LCD_NEXTION_HMI_STATEEDITNUMBER_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"

namespace StateEditNumber {

	void Activate(StateSettings::SettingDefinition setting);
	void TouchUpdate();
	void ReturnToLastState(void* ptr);
};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEEDITNUMBER_H_ */
