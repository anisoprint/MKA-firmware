/*
 * StateStatus2.h
 *
 *  Created on: 1 θών. 2018 γ.
 *      Author: Azarov
 */

#pragma once

#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateConnections {

  	void Back_Push(void *ptr);
  	void Wifi_Push(void *ptr);

	void Init();
	void Activate();
	void TouchUpdate();

};

#endif


