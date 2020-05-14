/*
 * sdstorage.h
 *
 *  Created on: 13 мая 2020 г.
 *      Author: Avazar
 */

#pragma once

#if HAS_SD_SUPPORT

class SdStorage {

public: /** Public Parameters */

	SDCard cards[SD_COUNT];

private: /** Private Parameters */

	int8_t activePrintSD;

public: /** Public Function */


	void init();
	void openAndPrintFile(const char * const path, uint8_t slot_index = 0);

	int8_t getActivePrintSD();

	inline void pauseSDPrint();
	inline bool isPaused();
	inline bool isPrinting();
	inline uint8_t percentDone();




};

extern SdStorage sdStorage;

#define IS_SD_PRINTING()  sdStorage.isPrinting()
#define IS_SD_PAUSED()    sdStorage.isPaused()

#else

#define IS_SD_PRINTING()  false
#define IS_SD_PAUSED()    false

#endif // HAS_SD_SUPPORT

