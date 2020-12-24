/*
 * StateMessage.h
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Azarov
 */

#pragma once

#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"

namespace StateKeyboard {
	void Activate(bool password, const char* header, const char* input, NexTouchEventCb cbOK, NexTouchEventCb cbCancel);
	void TouchUpdate();
	void GetInputToBuffer();
	void GetInputToBuffer(char* buffer, const uint16_t bufferSize);

};

#endif

