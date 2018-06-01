/*
 * StateStatus.h
 *
 *  Created on: 31 мая 2018 г.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_STATESTATUS_H_
#define SRC_LCD_NEXTION_HMI_STATESTATUS_H_

#if ENABLED(NEXTION_HMI)

#include "../nextion/library/Nextion.h"
#include "NextionConstants.h"

enum HeatingStatus { COOL, HEATING, COOLING, HOT  };

class StateStatus {
private:
	//Page
	static NexObject _page;
	//Build plate status
	static NexObject _tBuildPlateTemperature;
	static NexObject _tBuildPlateColorStatus;
	static NexObject _bBuildPlateTemperature;
	//Print head status
	static NexObject _tPrintHeadTemperature;
	static NexObject _tPrintHeadColorStatus;
	static NexObject _bPrintHeadTemperature;
	//Buttons
	static NexObject _bPrint;
	static NexObject _bMaintenance;

	static NexObject *_listenList[5];

  	static void PrintHeadTemperature_Push(void *ptr);
  	static void BuildPlateTemperature_Push(void *ptr);
  	static void Print_Push(void *ptr);
  	static void Maintenance_Push(void *ptr);
public:
	StateStatus();
	static void Init();
	static void Activate();
	static void DrawUpdate();
	static void TouchUpdate();

};

#endif

#endif /* SRC_LCD_NEXTION_HMI_STATESTATUS_H_ */
