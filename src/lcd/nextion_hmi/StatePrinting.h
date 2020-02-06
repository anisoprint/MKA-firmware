/*
 * StatePrinting.h
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEPRINTING_H_
#define SRC_LCD_NEXTION_HMI_STATEPRINTING_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"

#define TUNE_LIST_LENGTH 11

namespace StatePrinting {

	extern StateSettings::SettingDefinition TuneList[];

	void Control_Push(void *ptr);
	void Pause_Push(void *ptr);

	void Cancel_Push(void *ptr);

  	void OnEvent(HMIevent event, uint8_t eventArg);

	void Init();
	void Activate();
	void DrawUpdate();
	void TouchUpdate();

  	void DoneMessage_OK(void* ptr);
  	void DoneMessage_Again(void* ptr);

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEPRINTING_H_ */
