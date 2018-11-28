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

enum SettingType {HEADER, FLOAT, INT};

	struct SettingDefinition
	{
		const char *caption;
		SettingType type;
		void* setting;
		double minValue;
		double maxValue;
	};

	void FUp_Push(void *ptr);
	void FDown_Push(void *ptr);
	void FRow_Push(void *ptr);
	void OK_Push(void *ptr);

	void Init();
	void Activate();
	//void DrawUpdate();
	void TouchUpdate();

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATESETTINGS_H_ */
