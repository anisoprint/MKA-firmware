/*
 * StateFiles.h
 *
 *  Created on: 7 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEFILES_H_
#define SRC_LCD_NEXTION_HMI_STATEFILES_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateFiles {

	void FUp_Push(void *ptr);
	void FDown_Push(void *ptr);
	void FFile_Push(void *ptr);
	void FFolder_Push(void *ptr);
	void FFolderUp_Push(void *ptr);
	void FilesCancel_Push(void *ptr);

	void Init();
	void Activate();
	//void DrawUpdate();
	void TouchUpdate();

};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEFILES_H_ */
