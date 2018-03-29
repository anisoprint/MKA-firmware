/*
 * language_nextion.h
 *
 *  Created on: 29 мар. 2018 г.
 *      Author: Azarov
 */

#ifndef SRC_LCD_LANGUAGE_LANGUAGE_NEXTION_H_
#define SRC_LCD_LANGUAGE_LANGUAGE_NEXTION_H_


const uint8_t ZERO PROGMEM  = {0};

const char NXT_BUILDPLATE_TEMPERATURE[] PROGMEM  = {"Build plate temperature"};
const char NXT_PRINTHEAD_TEMPERATURE[] PROGMEM  = {"Print head temperature"};

const char NXT_PRINTING[] PROGMEM  =  {"Printing..."};


const char NXT_CANCELPRINT[] PROGMEM 			=	{"Cancel print"};
const char NXT_CANCELPRINT_CONFIRM[] PROGMEM 	=	{"Do you really want to cancel printing?"};
const char NXT_YES[] PROGMEM 					=	{"Yes"};
const char NXT_NO[] PROGMEM 					=	{"No"};
const char NXT_FINISHED[] PROGMEM 				=	{"Finished"};
const char NXT_DONE[] PROGMEM 					=	{"DONE!"};
const char NXT_PRINTAGAIN[] PROGMEM 			=	{"Print again"};
const char NXT_OK[] PROGMEM 					=	{"OK"};


#endif /* SRC_LCD_LANGUAGE_LANGUAGE_NEXTION_H_ */
