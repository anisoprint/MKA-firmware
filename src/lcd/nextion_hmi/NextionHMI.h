/*
 * NextionHMI.h
 *
 *  Created on: 31 ��� 2018 �.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_NEXTIONHMI_H_
#define SRC_LCD_NEXTION_HMI_NEXTIONHMI_H_

#if ENABLED(NEXTION_HMI)

#define NEXHMI_BUFFER_SIZE 256

#include "../nextion/library/Nextion.h"

namespace NextionHMI {

	extern NexObject headerText;
	extern NexObject headerIcon;

	extern NexObject wifiStatus;
	extern NexObject ethernetStatus;
	extern NexObject acStatus;

	extern NexObject statusIcon0;
	extern NexObject statusIcon1;
	extern NexObject statusIcon2;

	extern NexObject* statusIcons[];

	extern uint8_t wizardData;
	extern int16_t pageData;

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

	#if HAS_SD_SUPPORT && PIN_EXISTS(SD_DETECT)
	void UpdateSDIcon();
	#endif

	#if HAS_SD_SUPPORT
	void UploadFirmwareFromSD(uint8_t sd_slot);
	#endif

	void SetNetworkStatus(bool _wifiEnabled, bool _wifiConnected, bool _ethernetConnected, bool _acConnected);

	void UploadFirmwareFromSerial(uint32_t tftSize, uint8_t serialPort);

	uint8_t GetActiveState();

	void RaiseEvent(HMIevent event, uint8_t eventArg=0, const char *eventMsg = 0);
	void WaitOk_Push(void* ptr);
};


#endif

#endif /* SRC_LCD_NEXTION_HMI_NEXTIONHMI_H_ */
