/*
 * StateFiles.h
 *
 *  Created on: 7 θών. 2018 γ.
 *      Author: Azarov
 */

#pragma once


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateWifiList {

	void Up_Push(void *ptr);
	void Down_Push(void *ptr);
	void Network_Push(void *ptr);
	void Cancel_Push(void *ptr);

	void NetKey_OkPush(void *ptr);
	void NetKey_CancelPush(void *ptr);

	void Init();
	void Activate();
	void TouchUpdate();

};

#endif

