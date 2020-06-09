/*
 * sdstorage.cpp
 *
 *  Created on: 13 мая 2020 г.
 *      Author: Avazar
 */

#include "../../MK4duo.h"

SdStorage sdStorage;

void SdStorage::init() {
	cards[0].init(SD0_SS_PIN, SD0_SPEED_DIVISOR, SD_DETECT_PIN);
	#if SD_COUNT>1
	cards[1].init(SD1_SS_PIN, SD1_SPEED_DIVISOR, SD_DETECT_PIN_1);
	#endif
	_activePrintSD = -1;
	_completedPrintSD = -1;
	_savingSD = -1;
}

void SdStorage::openAndPrintFile(uint8_t slot, const char * const path, int32_t index = -1) {
  if (IS_SD_PRINTING())
  {
    stepper.synchronize();
    cards[_activePrintSD].closeFile();
    cards[_activePrintSD].setPrinting(false);
  }

  _activePrintSD = -1;
  _completedPrintSD = -1;

  SERIAL_MV("Open file: ", path);
  SERIAL_EM(" and start print.");

  if (!cards[slot].selectFile(path)) return;
  if (index!=-1) cards[slot].setIndex(index);

  startSelectedFilePrint(slot);

}


void SdStorage::startSelectedFilePrint(uint8_t slot) {

  if (!cards[slot].isFileOpen()) return;

  Printer::currentLayer  = 0,
  Printer::maxLayer = -1;

  cards[slot].startFilePrint();
  _activePrintSD = slot;
  _completedPrintSD = -1;
  print_job_counter.start();

  #if ENABLED(NEXTION_HMI)
    StatePrinting::Activate();
  #endif

  #if HAS_POWER_CONSUMPTION_SENSOR
    powerManager.startpower = powerManager.consumption_hour;
  #endif
}

void SdStorage::endFilePrint() {
  cards[_activePrintSD].endFilePrint();
  _activePrintSD = -1;
  _completedPrintSD = -1;
}


void SdStorage::printFileHasFinished() {
  if (_activePrintSD!=-1) {
    cards[_activePrintSD].fileHasFinished();
    _completedPrintSD = _activePrintSD;
    _activePrintSD = -1;
    print_job_counter.stop();
  }

}

void SdStorage::mountAll() {
  for(int i=0; i<SD_COUNT; i++) {
    cards[i].mount();
  }
}

void SdStorage::processAutoreport() {
  for(int i=0; i<SD_COUNT; i++) {
    if (cards[i].isAutoreport()) cards[i].print_status();
  }
}



