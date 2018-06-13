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
  PRINTER_KILLED,				//-					msg
  HEATING_STARTED_EXTRUDER, 	//Heater number
  HEATING_STARTED_BUILDPLATE,	//Heater number
  SD_PRINT_FINISHED				//-
};

#endif



#endif /* SRC_LCD_NEXTION_HMI_HMIEVENTS_H_ */
