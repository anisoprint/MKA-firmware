/*
 * StateStatus2.h
 *
 *  Created on: 1 ���. 2018 �.
 *      Author: Azarov
 */

#pragma once

#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateAuraConnect {

  	void Close_Push(void *ptr);
  	void OK_Push(void *ptr);

  	void ServerUrl_Push(void *ptr);
  	void ServerSecurityCode_Push(void *ptr);

  	void ServerUrl_OkPush(void *ptr);
  	void ServerSecurityCode_OkPush(void *ptr);
  	void Dialog_CancelPush(void *ptr);

	void Init();
	void Activate();
	void TouchUpdate();


};

#endif


