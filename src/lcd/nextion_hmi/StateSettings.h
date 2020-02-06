/*
 * StateFiles.h
 *
 *  Created on: 7 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATESETTINGS_H_
#define SRC_LCD_NEXTION_HMI_STATESETTINGS_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"



namespace StateSettings {

#define SETTINGS_LIST_LENGTH 18

enum SettingType {HEADER, FLOAT, INT16, INT8};

	struct SettingDefinition
	{
		const char *caption;
		SettingType type;
		void* setting;
		double minValue;
		double maxValue;
	};

	extern SettingDefinition SettingsList[];


	void FUp_Push(void *ptr);
	void FDown_Push(void *ptr);
	void FRow_Push(void *ptr);
	void OK_Push(void *ptr);

  	void EditNumber_OK_Push(void *ptr);
  	void EditNumber_Cancel_Push(void *ptr);
  	void EditNumber_MsgOk_Push(void *ptr);

	void Init();
	void Activate(StateSettings::SettingDefinition* settings, uint32_t settingsLength, NexTouchEventCb cbOK);
	void Activate(uint32_t setting_index);
	//void DrawUpdate();
	void TouchUpdate();

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATESETTINGS_H_ */
