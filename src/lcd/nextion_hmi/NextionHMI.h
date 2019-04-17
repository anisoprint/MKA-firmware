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

	extern uint8_t wizardData;
	extern uint8_t pageData;

    extern uint16_t autoPreheatTempHotend;
    extern uint16_t autoPreheatTempBed;
    extern uint8_t lcdBrightness;

	extern HMIevent lastEvent;
	extern uint8_t  lastEventArg;

	extern char buffer[256];

	void Init();
	void DrawUpdate();
	void TouchUpdate();
	void ActivateState(uint8_t state_id);
	void ShowState(uint8_t state_id);

	void SetBrightness(uint8_t brightness);

	void ShowStartScreen(const char* header, const char* message);

	#if HAS_SDSUPPORT && PIN_EXISTS(SD_DETECT)
	void UpdateSDIcon();
	#endif

	#if HAS_SDSUPPORT
	void UploadFirmwareFromSD();
	#endif

	void UploadFirmwareFromSerial(uint32_t tftSize);

	uint8_t GetActiveState();

	void RaiseEvent(HMIevent event, uint8_t eventArg=0, const char *eventMsg = 0);
	void WaitOk_Push(void* ptr);
};


#endif

#endif /* SRC_LCD_NEXTION_HMI_NEXTIONHMI_H_ */
