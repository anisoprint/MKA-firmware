/*
 * StateMessage.h
 *
 *  Created on: 9 ���. 2018 �.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEMESSAGE_H_
#define SRC_LCD_NEXTION_HMI_STATEMESSAGE_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"

#define MESSAGE_DIALOG 5
#define MESSAGE_DIALOG_OVER 6
#define MESSAGE_EXTERNAL 10
#define MESSAGE_WARNING 50
#define MESSAGE_ERROR 200
#define MESSAGE_CRITICAL_ERROR 255

namespace StateMessage {

	void Init();
	void ActivatePGM(uint8_t priority, uint8_t icon, const char *header_P, const char *message_P, uint8_t numButtons, const char *txtButtonRight_P, NexTouchEventCb cbRight, const char *txtButtonLeft_P, NexTouchEventCb cbLeft, uint8_t picture=0);
	void ActivatePGM_M(uint8_t priority, uint8_t icon, const char *header_P, const char *message, uint8_t numButtons, const char *txtButtonRight_P, NexTouchEventCb cbRight, const char *txtButtonLeft_P, NexTouchEventCb cbLeft, uint8_t picture=0);
	void UpdateMessage(const char *message);

	void TouchUpdate();
	void ReturnToLastState(void* ptr);
	void RetryHeaterAndReturnToLastState(void* ptr);

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEMESSAGE_H_ */
