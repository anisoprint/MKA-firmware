/*
 * StateMenu.h
 *
 *  Created on: 10 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATEMENU_H_
#define SRC_LCD_NEXTION_HMI_STATEMENU_H_


#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"


namespace StateMenu {

	void TouchUpdate();

	void ActivateMaintenance(void *ptr=0);
	void MaintenanceMovePH(void *ptr);
	void MaintenanceBack(void *ptr);


};

#endif


#endif /* SRC_LCD_NEXTION_HMI_STATEMENU_H_ */
