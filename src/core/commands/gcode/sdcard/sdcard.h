/**
 * MK4duo Firmware for 3D Printer, Laser and CNC
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 Alberto Cotronei @MagoKimbra
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * mcode
 *
 * Copyright (C) 2017 Alberto Cotronei @MagoKimbra
 */

#if HAS_SD_SUPPORT

  #define CODE_M20
  #define CODE_M21
  #define CODE_M22
  #define CODE_M23
  #define CODE_M24
  #define CODE_M25
  #define CODE_M26
  #define CODE_M27
  #define CODE_M28
  #define CODE_M29
  #define CODE_M30
  #define CODE_M32
  #define CODE_M524

  /**
   * M20: List SD card to serial output
   */
  inline void gcode_M20(void) {
    int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
    if (!commands.get_target_sdcard(s)) return;

    SERIAL_EM(MSG_BEGIN_FILE_LIST);
    sdStorage.cards[s].ls();
    SERIAL_EM(MSG_END_FILE_LIST);
  }

  /**
   * M21: Init SD Card
   */
  inline void gcode_M21(void) {
    int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
    if (!commands.get_target_sdcard(s)) return;

    sdStorage.cards[s].mount();
  }

  /**
   * M22: Release SD Card
   */
  inline void gcode_M22(void) {
    int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
	  if (!commands.get_target_sdcard(s)) return;

	  sdStorage.cards[s].unmount();
  }

  /**
   * M23: Select a file
   */
  inline void gcode_M23(void) {
    int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
    if (!commands.get_target_sdcard(s)) return;

    sdStorage.cards[s].selectFile(parser.string_arg);
  }

  /**
   * M24: Start or Resume SD Print
   */
  inline void gcode_M24(void) {

    int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
    if (!commands.get_target_sdcard(s)) return;

    #if HAS_SD_RESTART
      sdStorage.cards[s].delete_restart_file();
    #endif

    #if ENABLED(PARK_HEAD_ON_PAUSE) && ENABLED(ADVANCED_PAUSE_FEATURE)
      resume_print();
    #endif

	#if ENABLED(NEXTION_HMI)
      if (printer.getStatus()==WaitingToPause || printer.getStatus()==Paused)
    	  PrintPause::ResumePrint();
      else if (printer.getStatus()==Idle)
      {
    		sdStorage.startSelectedFilePrint(s);
      }
	#else
      sdStorage.startSelectedFilePrint(s);
	#endif


  }

  /**
   * M25: Pause SD Print
   */
  void gcode_M25(void) {

	#if ENABLED(NEXTION_HMI)
	  PrintPause::PausePrint();
	#else
	    sdStorage.pauseSDPrint();
	    print_job_counter.pause();
	    SERIAL_STR(PAUSE);
	    SERIAL_EOL();

	    #if ENABLED(PARK_HEAD_ON_PAUSE)
	      commands.enqueue_and_echo_P(PSTR("M125")); // Must be enqueued with pauseSDPrint set to be last in the buffer
	    #endif
	#endif


  }

  /**
   * M26: Set SD Card file index
   */
  inline void gcode_M26(void) {
    int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
    if (!commands.get_target_sdcard(s)) return;

    if (sdStorage.cards[s].isMounted() && parser.seen('S')) {
      sdStorage.cards[s].setIndex(parser.value_long());
    }
  }

  /**
   * M27: Get SD Card status
   *      OR, with 'S<bool>' set the SD status auto-report.
   *      OR, with 'C' get the current filename.
   */
  inline void gcode_M27() {
    int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
    if (!commands.get_target_sdcard(s)) return;

    if (parser.seen('C'))
      SERIAL_EMT("Current file: ", sdStorage.cards[s].fileName);
    else if (parser.seenval('S'))
      sdStorage.cards[s].setAutoreport(parser.value_bool());
    else
      sdStorage.cards[s].print_status();
  }

  /**
   * M28: Start SD Write
   * M28 P[slot] [Filename]
   * OR Fast file transfer
   * M28.1 P[slot] S[size] [Filename]
   */
  inline void gcode_M28(void) {
    int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
    if (!commands.get_target_sdcard(s)) return;

  #if ENABLED(M28_FAST_UPLOAD)
    if (parser.subcode == 1)
    {
      if (Commands::current_command_port < 0 || Commands::current_command_port >= NUM_SERIAL)
      {
        SERIAL_LM(ER, MSG_ERR_M28_SERIAL);
        return;
      }
      if (parser.seen('S')) {
        uint32_t size = parser.value_ulong();
        sdStorage.receiveFile(Commands::current_command_port, s, parser.string_arg, size);
      }
      else return;
    }
    else sdStorage.startSaving(s, parser.string_arg, false);

  #else
    sdStorage.startSaving(s, parser.string_arg, false);
  #endif

  }

  /**
   * M29: Stop SD Write
   * Processed in write to file routine above
   */
  inline void gcode_M29(void) {
    sdStorage.finishSaving();
  }

  /**
   * M30 <filename>: Delete SD Card file
   */
  inline void gcode_M30(void) {
    int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
    if (!commands.get_target_sdcard(s)) return;

    if (sdStorage.cards[s].isMounted()) {
      sdStorage.cards[s].closeFile();
      sdStorage.cards[s].deleteFile(parser.string_arg);
    }
  }

  /**
   * M32: Select file and start SD print
   */
  inline void gcode_M32(void) {

    int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
    if (!commands.get_target_sdcard(s)) return;

    char* namestartpos = parser.string_arg ; // default name position

    sdStorage.openAndPrintFile(s, namestartpos, 0);

  }



/**
 * M524: Abort the current SD print job (started with M24)
 */
inline void gcode_M524() {
  if (IS_SD_PRINTING() || IS_SD_PAUSED()
		  || printer.getStatus() == Printing
		  || printer.getStatus() == WaitingToPause
		  || printer.getStatus() == Pausing
		  || printer.getStatus() == Paused
		  || printer.getStatus() == Resuming ) {
    printer.setCancelPrint(true);
  }
}

  #if ENABLED(SDCARD_SORT_ALPHA) && ENABLED(SDSORT_GCODE)

    #define CODE_M34

    /**
     * M34: Set SD Card Sorting Options
     */
    inline void gcode_M34(void) {
      int8_t s = parser.seen('P') ? parser.value_int() : 0; // selected sd card
      if (!commands.get_target_sdcard(s)) return;

      if (parser.seen('S')) sdStorage.cards[s].setSortOn(parser.value_bool());
      if (parser.seenval('F')) {
        const int v = parser.value_long();
        sdStorage.cards[s].setSortFolders(v < 0 ? -1 : v > 0 ? 1 : 0);
      }
      //if (parser.seen('R')) card.setSortReverse(parser.value_bool());
    }

  #endif // SDCARD_SORT_ALPHA && SDSORT_GCODE

#endif // HAS_SD_SUPPORT
