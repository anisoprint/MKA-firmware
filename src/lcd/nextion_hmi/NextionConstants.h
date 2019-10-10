/*
 * NextionConstants.h
 *
 *  Created on: 31 мая 2018 г.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_NEXTIONCONSTANTS_H_
#define SRC_LCD_NEXTION_HMI_NEXTIONCONSTANTS_H_



//Pages
#define PAGE_STATUS 1
#define PAGE_TEMPERATURE 2
#define PAGE_FILES 3
#define PAGE_FILEINFO 4
#define PAGE_PRINTING 5
#define PAGE_MESSAGE 6
#define PAGE_WIZARD 7
#define PAGE_MENU 8
#define PAGE_MOVEMENT 9
#define PAGE_SETTINGS 10
#define PAGE_ABOUT 11
#define PAGE_WIZARDZ 12
#define PAGE_DINFO 13
#define PAGE_C_NUMBER 14

//Icons
#define NEX_ICON_READY 1
#define NEX_ICON_PAUSE 2
#define NEX_ICON_FINISHED 3
#define NEX_ICON_INPROGRESS1 4
#define NEX_ICON_INPROGRESS2 5
#define NEX_ICON_INPROGRESS3 6
#define NEX_ICON_MAINTENANCE 7
#define NEX_ICON_MOVEMENT 8
#define NEX_ICON_SETTINGS 9
#define NEX_ICON_ERROR 10
#define NEX_ICON_WARNING 11
#define NEX_ICON_INFO 12
#define NEX_ICON_TEMPERATURE 13
#define NEX_ICON_FILES 14
#define NEX_ICON_BLANK_HEADER 15

#define NEX_ICON_NO_SD 16
#define NEX_ICON_SD 17

#define NEX_ICON_FILE_BLANK 18
#define NEX_ICON_FILE_FOLDER 19
#define NEX_ICON_FILE_BACK 20

#define NEX_ICON_FILES_DOWN 21
#define NEX_ICON_FILES_UP   22
#define NEX_ICON_FILES_DOWN_GREY 23
#define NEX_ICON_FILES_UP_GREY  24

#define NEX_ICON_FILE_GCODE_AURA 25
#define NEX_ICON_FILE_GCODE 26

#define NEX_ICON_DONE 27



/*	      char UBL_LCD_GCODE[20];
	      commands.enqueue_and_echo_P(PSTR("G28"));
	      #if HAS_TEMP_BED
	        sprintf_P(UBL_LCD_GCODE, PSTR("M190 S%i"), custom_bed_temp);*/

#endif /* SRC_LCD_NEXTION_HMI_NEXTIONCONSTANTS_H_ */
