/*
 * StateWizard.h
 *
 *  Created on: 14 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEWIZARDZ_H_
#define SRC_LCD_NEXTION_HMI_STATEWIZARDZ_H_


#if ENABLED(NEXTION_HMI)



#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateWizardZ {

  	void Movement_Push(void *ptr);
  	void Babystep_Push(void *ptr);

  	void InitMovement();
  	void InitBabystep();
  	void DrawUpdate();
	void ZOffsetS2(void *ptr=0);
	void CompZOffsetS2(void *ptr=0);
	void BuildPlateS3(void *ptr=0);
	void BuildPlateS7(void *ptr=0);

	void BabystepZ(void *ptr=0);
	void BabystepZ_Save(void *ptr=0);
	void BabystepZ_Cancel(void *ptr=0);

	void TouchUpdate();

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEWIZARDZ_H_ */
