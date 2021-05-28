/*
 * sdstorage.cpp
 *
 *  Created on: 13 мая 2020 г.
 *      Author: Avazar
 */

#include "../../MK4duo.h"

SdStorage sdStorage;

void SdStorage::init() {
	cards[0].init(pins[0], spi_speed_divisors[0], detect_pins[0]);
	#if SD_COUNT>1
	cards[1].init(pins[1], spi_speed_divisors[1], detect_pins[1]);
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

  if (strstr(path, ".ac") != nullptr)
  {
	  cards[slot].setroot();
  }

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
  printer.setStatus(Printing);
  SERIAL_LMT(JOB, MSG_JOB_START, getActivePrintSDCard()->fileName);

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
  }

}

void SdStorage::mountAll() {
  for(int i=0; i<SD_COUNT; i++) {
    cards[i].mount();
  }

  if (pins[INTERNAL_SD_STORAGE_INDEX] > NoPin)
  {
	  //create system directories
	  if (cards[INTERNAL_SD_STORAGE_INDEX].isMounted())
	  {
		  if (!cards[INTERNAL_SD_STORAGE_INDEX].exists(".ac"))
		  {
			  cards[INTERNAL_SD_STORAGE_INDEX].makeDirectory(".ac");
		  }
	  }
	  else
	  {
			#if ENABLED(NEXTION_HMI)
			  NextionHMI::RaiseEvent(HMIevent::INTERNAL_SD_WARNING, 0, MSG_INTERNAL_SD_MOUNT_FAIL);
			#endif
	  }
  }

}

void SdStorage::clearInternalStorageAc() {
	if (cards[INTERNAL_SD_STORAGE_INDEX].isMounted())
	{
		cards[INTERNAL_SD_STORAGE_INDEX].chdir(".ac");
		cards[INTERNAL_SD_STORAGE_INDEX].deleteAllFilesInWorkDirectory();
		cards[INTERNAL_SD_STORAGE_INDEX].setroot();
	}
}

void SdStorage::processAutoreport() {
  for(int i=0; i<SD_COUNT; i++) {
    if (cards[i].isAutoreport()) cards[i].print_status();
  }
}

#if ENABLED (M28_FAST_UPLOAD)

void SdStorage::receiveFile(uint8_t serialPort, uint8_t slot, const char * const path, uint32_t size) {

  SERIAL_PORT(serialPort);
  ZERO(NextionHMI::buffer);
  sprintf_P(NextionHMI::buffer, PSTR(MSG_RECEIVING_FILE), 0, size/1024);
  StateMessage::ActivatePGM_M(MESSAGE_DIALOG_OVER, NEX_ICON_INFO, MSG_AURA_CONNECT, NextionHMI::buffer, 1, PSTR(MSG_PLEASE_WAIT), 0, 0, 0);

  if (!cards[slot].startWrite(path, true))
  {
    SERIAL_EMV("RESULT:", "ERROR_OPEN_FILE");
    SERIAL_PORT(-1);
    StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, PSTR(MSG_ERROR), PSTR(MSG_ERROR_FILE_WRITE_OPEN), 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
    return;
  }

  switch (serialPort) {
    case 0:
      fileTransfer.begin(MKSERIAL1);
      break;
    case 1:
      fileTransfer.begin(MKSERIAL2);
      break;
    default:
      SERIAL_PORT(-1);
      StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, PSTR(MSG_ERROR), PSTR(MSG_ERROR), 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);

      return;
  }

  servo[Tools::cut_servo_id].detach();
  bool wasReportSuspended = printer.isSuspendAutoreport();
  printer.setSuspendAutoreport(true);


  SERIAL_EMV("RESULT:", "READY");
  uint32_t old_transfered = 0;
  uint32_t transfered = 0;
  uint8_t retries = 0;

  watch_t timeoutWatch = watch_t(10000);

  while (transfered<size){
    if(fileTransfer.available())
    {
      SERIAL_EMV("RESULT:", "ok");
      timeoutWatch.start();
      size_t written = cards[slot].write(fileTransfer.rxBuff, fileTransfer.bytesRead);
      if (written==-1 || written!=fileTransfer.bytesRead)
      {
        cards[slot].finishWrite();
        cards[slot].deleteFile(path);
        SERIAL_EMV("RESULT:", "ERROR_WRITE_FILE");
        SERIAL_PORT(-1);
        servo[Tools::cut_servo_id].reattach();
        printer.setSuspendAutoreport(wasReportSuspended);
        StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, PSTR(MSG_ERROR), PSTR(MSG_ERROR_FILE_WRITE), 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
        return;
      }
      transfered+=fileTransfer.bytesRead;

      if (old_transfered - transfered > 10000)
      {
          old_transfered = transfered;
          ZERO(NextionHMI::buffer);
          sprintf_P(NextionHMI::buffer, PSTR(MSG_RECEIVING_FILE), transfered/1024, size/1024);
          StateMessage::UpdateMessage(NextionHMI::buffer);
      }

      retries = 0;
      printer.check_periodical_actions();

    }
    else
    {
    	if(fileTransfer.status < 0)
		{
		  if (retries==MAX_RETRIES) {
			cards[slot].finishWrite();
			cards[slot].deleteFile(path);
			SERIAL_EMV("RESULT:", "ERROR_TRANSFER");
			SERIAL_PORT(-1);
			servo[Tools::cut_servo_id].reattach();
	        printer.setSuspendAutoreport(wasReportSuspended);
			StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, PSTR(MSG_ERROR), PSTR(MSG_ERROR_FILE_TRANSFER), 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
			return;
		  }
		  retries++;
		  if(fileTransfer.status == -1)
			SERIAL_EMV("RESULT:", "RESEND_CRC_ERROR");
		  else if(fileTransfer.status == -2)
			SERIAL_EMV("RESULT:", "RESEND_PAYLOAD_ERROR");
		  else if(fileTransfer.status == -3)
			SERIAL_EMV("RESULT:", "RESEND_STOPBYTE_ERROR");
		}
    	else
    	{
    		if (timeoutWatch.elapsed())
    		{
    			cards[slot].finishWrite();
    			cards[slot].deleteFile(path);
    			SERIAL_EMV("RESULT:", "ERROR_TRANSFER");
    			SERIAL_PORT(-1);
    			servo[Tools::cut_servo_id].reattach();
    	        printer.setSuspendAutoreport(wasReportSuspended);
    			StateMessage::ActivatePGM_M(MESSAGE_ERROR, NEX_ICON_ERROR, PSTR(MSG_ERROR), PSTR(MSG_ERROR_FILE_TIMEOUT), 1, PSTR(MSG_OK), StateMessage::ReturnToLastState, 0, 0);
    			SERIAL_FLUSH();
    			netBridgeManager.SendReconnect();
    			return;

    		}
    	}
    }
  }

  cards[slot].finishWrite();
  SERIAL_EMV("RESULT:", "FINISH");
  servo[Tools::cut_servo_id].reattach();

  ZERO(NextionHMI::buffer);
  sprintf_P(NextionHMI::buffer, PSTR(MSG_RECEIVING_FILE), 0, size/1024);
  printer.setSuspendAutoreport(wasReportSuspended);
  SERIAL_PORT(-1);

}

#endif



