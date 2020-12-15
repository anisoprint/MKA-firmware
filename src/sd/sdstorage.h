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

	pin_t pins[SD_COUNT];
	pin_t detect_pins[SD_COUNT];
	uint8_t spi_speed_divisors[SD_COUNT];

	SDCard cards[SD_COUNT];

private: /** Private Parameters */

	int8_t _savingSD;
	int8_t _activePrintSD;
	int8_t _completedPrintSD;
	bool _abortPrinting;

#if ENABLED (M28_FAST_UPLOAD)
	SerialTransfer fileTransfer;
#endif


public: /** Public Function */

#if ENABLED (M28_FAST_UPLOAD)

  #define MAX_RETRIES 8

	void receiveFile(uint8_t serialPort, uint8_t slot, const char * const path, uint32_t size);

#endif

	void init();
	void openAndPrintFile(uint8_t slot, const char * const path, int32_t index);
	void startSelectedFilePrint(uint8_t slot = 0);
	void printFileHasFinished();
	void endFilePrint();

	void mountAll();
	void processAutoreport();

	void clearInternalStorageAc();

	inline SDCard* getActivePrintSDCard() { return &cards[_activePrintSD]; }

	inline int8_t getActivePrintSD() { return _activePrintSD; };
	inline int8_t getSavingSD() { return _savingSD; };
	inline int8_t getCompletedPrintSD() { return _completedPrintSD; };

	inline void pauseSDPrint() { cards[_activePrintSD].pauseSDPrint();};
	inline void resumeSDPrint() { if (_activePrintSD!=-1) cards[_activePrintSD].startFilePrint(); };
	inline bool isPaused() { return (_activePrintSD!=-1 && cards[_activePrintSD].isPaused()); };
	inline bool isPrinting() { return (_activePrintSD!=-1 && cards[_activePrintSD].isPrinting()); };
	inline uint8_t percentDone() { return _activePrintSD!=-1 ? cards[_activePrintSD].percentDone() : 0; };
	inline float fractionDone() { return _activePrintSD!=-1 ? cards[_activePrintSD].fractionDone() : 0; };

	inline void startSaving(uint8_t slot, const char * const path, const bool silent=false) { if (cards[slot].startWrite(path, silent)) _savingSD = slot; };
	inline void finishSaving() {
		if (_savingSD!=-1) {
			cards[_savingSD].finishWrite();
			_savingSD=-1;
		}
	};
	inline bool isSaving() { return _savingSD!=-1; };

	inline bool isSdInserted(uint8_t slot = 0) { return cards[slot].isInserted(); };


  // Card flag bit 1 Abortprinting
  FORCE_INLINE void setAbortSDprinting(const bool onoff) { _abortPrinting = onoff; }
  FORCE_INLINE bool isAbortSDprinting() { return _abortPrinting; }

};

extern SdStorage sdStorage;

#define IS_SD_PRINTING()  sdStorage.isPrinting()
#define IS_SD_PAUSED()    sdStorage.isPaused()

#else

#define IS_SD_PRINTING()  false
#define IS_SD_PAUSED()    false

#endif // HAS_SD_SUPPORT

