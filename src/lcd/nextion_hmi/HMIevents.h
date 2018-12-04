/*
 * HMIevents.h
 *
 *  Created on: 9 θών. 2018 γ.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_HMIEVENTS_H_
#define SRC_LCD_NEXTION_HMI_HMIEVENTS_H_


#if ENABLED(NEXTION_HMI)

enum HMIevent {					//Eventarg
  NONE,
  SWITCHED_OFF,					//					msg
  PRINTER_KILLED,				//-					msg
  TEMPERATURE_ERROR,			//Heater number		msg
  SD_ERROR,						//Error code		msg
  ERROR,						//-					msg
  WAIT_FOR_INPUT,				//-					msg
  HEATING_STARTED_EXTRUDER, 	//Heater number
  HEATING_STARTED_BUILDPLATE,	//Heater number
  HEATING_FINISHED,				//
  SD_PRINT_FINISHED,			//-
  PRINT_PAUSING,				//-
  PRINT_PAUSED,					//-
  PRINT_PAUSE_SCHEDULED,		//-
  PRINT_PAUSE_UNSCHEDULED,		//-
  PRINT_PAUSE_RESUMING,			//-
  PRINT_PAUSE_RESUMED,			//-
  PRINT_CANCELLING,				//-
  PRINT_CANCELLED				//-
};

#endif



#endif /* SRC_LCD_NEXTION_HMI_HMIEVENTS_H_ */
