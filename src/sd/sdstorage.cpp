/*
 * sdstorage.cpp
 *
 *  Created on: 13 мая 2020 г.
 *      Author: Avazar
 */

#include "../../MK4duo.h"

SdStorage sdStorage;

void SdStorage::init() {
	cards[0].init(SD0_SS_PIN, SD0_SPEED_DIVISOR);
	#if SD_COUNT>1
	cards[1].init(SD1_SS_PIN, SD1_SPEED_DIVISOR);
	#endif
}

void SdStorage::openAndPrintFile(const char *const path, uint8_t slot_index) {
    int8_t index = parser.seen('H') ? parser.value_int() : 0; // hotend being updated

    if (!commands.get_target_heater(h)) return;
}

int8_t SdStorage::getActivePrintSD() {
}

inline void SdStorage::pauseSDPrint() {
}

inline bool SdStorage::isPaused() {
}

inline bool SdStorage::isPrinting() {
}

inline uint8_t SdStorage::percentDone() {
}
