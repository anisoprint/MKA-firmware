/**
 * MK4duo Firmware for 3D Printer, Laser and CNC
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (c) 2019 Alberto Cotronei @MagoKimbra
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

#pragma once

/**
 * commands.h
 *
 * Copyright (c) 2019 Alberto Cotronei @MagoKimbra
 */

#include "parser.h"

#define PS_NORMAL 0
#define PS_EOL    1
#define PS_QUOTED 2
#define PS_PAREN  3
#define PS_ESC    4

struct gcode_t {
  char    gcode[MAX_CMD_SIZE];  // Char for gcode
  bool    send_ok = true;       // Send "ok" after commands by default
  int8_t  s_port  = -1;         // Serial port for print information:
                                //    -1 for all port
                                //    -2 for SD or null port
};

class Commands {

  public: /** Constructor */

    Commands() {}

  public: /** Public Parameters */

    /**
     * GCode Command Buffer Ring
     * A simple ring buffer of BUFSIZE command strings.
     *
     * Commands are copied into this buffer by the command injectors
     * (immediate, serial, sd card) and they are processed sequentially by
     * the main loop. The process_next function parses the next
     * command and hands off execution to individual handler functions.
     */
    static Circular_Queue<gcode_t, BUFSIZE> buffer_ring;

    static int8_t current_command_port; // Serial port for print information:
    									//    -1 for all port
    									//    -2 for SD or null port

    /**
     * GCode line number handling. Hosts may opt to include line numbers when
     * sending commands to MK4duo, and lines will be checked for sequentiality.
     * M110 N<int> sets the current line number.
     */
    static long gcode_last_N[NUM_SERIAL];

  private: /** Private Parameters */

    static long gcode_N;

    static uint16_t serial_count[NUM_SERIAL];

    /**
     * Next Injected Command pointer. Nullptr if no commands are being injected.
     * Used by MK4duo internally to ensure that commands initiated from within
     * are enqueued ahead of any pending serial or sd card
     */
    static PGM_P injected_commands_front_P;
    static PGM_P injected_commands_rear_P;

  public: /** Public Function */

    /**
     * Send a "Resend: nnn" message to the host to
     * indicate that a command needs to be re-sent.
     */
    static void flush_and_request_resend();

    /**
     * Add to the buffer ring the next command from:
     *  - The command-injection queue (injected_commands_P)
     *  - The active serial input (usually USB)
     *  - The SD card file being actively printed
     */
    static void get_available();

    /**
     * Get the next command in the buffer_ring, optionally log it to SD, then dispatch it
     */
    static void advance_queue();

    /**
     * Clear the MK4duo command buffer_ring
     */
    static void clear_queue();

    /**
     * Enqueue one or many commands to run from program memory before the common queue.
     * Aborts the current queue, if any.
     * Note: process_injected_front() will process them.
     */
    static void inject_front_P(PGM_P const pgcode);

    /**
     * Enqueue one or many commands to run from program memory at the end of the current queue.
     * Aborts the current queue, if any.
     * Note: process_injected_rear() must be called repeatedly to drain the commands afterwards
     */
    static void inject_rear_P(PGM_P const pgcode);

    /**
     * Enqueue and return only when commands are actually enqueued
     */
    static void enqueue_one_now(const char * cmd);

    /**
     * Enqueue from program memory and return only when commands are actually enqueued
     */
    static void enqueue_now_P(PGM_P const pgcode);

    /**
     * Run a series of commands, bypassing the command queue to allow
     * G-code "macros" to be called from within other G-code handlers.
     */
    static void process_now(char * gcode);
    static void process_now_P(PGM_P pgcode);

    /**
     * Set XYZE mechanics.destination and mechanics.feedrate_mm_s from the current GCode command
     *
     *  - Set mechanics.destination from included axis codes
     *  - Set to current for missing axis codes
     *  - Set the mechanics.feedrate_mm_s, if included
     */
    static void get_destination();

    /**
     * Set target tool from the T parameter or the active_tool
     *
     * Returns TRUE if the target is invalid
     */
    static bool get_target_tool(const uint16_t code);

    /**
     * Set target driver from the T parameter or the active_driver
     *
     * Returns TRUE if the target is invalid
     */
    static bool get_target_driver(const uint16_t code);

    /**
     * Set target heather from the H parameter
     *
     * Returns NULL if the target is invalid
     */
    static bool get_target_heater(int8_t &h);

    /**
     * Returns TRUE if the target is valid
     */
    static bool get_target_sdcard(int8_t s);

    #if HAS_FANS
      /**
       * Set target fan from the P parameter
       *
       * Returns TRUE if the target is invalid
       */
      static bool get_target_fan(uint8_t &f);
    #endif

  private: /** Private Function */

    /**
     * Send an "ok" message to the host, indicating
     * that a command was successfully processed.
     *
     * If ADVANCED_OK is enabled also include:
     *   N<int>  Line number of the command, if any
     *   P<int>  Planner space remaining
     *   B<int>  Block queue space remaining
     */
    static void ok_to_send();

    /**
     * Get all commands waiting on the serial port and queue them.
     * Exit when the buffer is full or when no more characters are
     * left on the serial port.
     */
    static void get_serial();

    /**
     * Get commands from the SD Card until the command buffer is full
     * or until the end of the file is reached. The special character '#'
     * can also interrupt buffering.
     */
    #if HAS_SD_SUPPORT
      static void get_sdcard();
    #endif

    /**
     * Process a single command and dispatch it to its handler
     * This is called from the main loop()
     */
    static void process_next();

    static void unknown_warning();

    static void gcode_line_error(PGM_P err, const uint8_t tmp_port, bool send_error_message=true);

    /**
     * Enqueue with Serial Echo
     * Return true on success
     */
    static bool enqueue_one(const char * cmd);

    /**
     * Copy a command from RAM into the main command buffer.
     * Return true if the command was successfully added.
     * Return false for a full buffer, or if the 'command' is a comment.
     */
    static bool enqueue(const char * cmd, bool say_ok=false, int8_t port=-2);

    /**
     * Process the next "immediate" command
     */
    static bool process_injected_front();

    /**
     * Inject the next "immediate" command, when possible, onto the rear of the buffer_ring.
     * Return true if any immediate commands remain to inject.
     */
    static bool process_injected_rear();

    /**
     * Process the next "immediate" command
     */
    static bool process_without_queue(const char * cmd);

    /**
     * Process parsed gcode and execute command
     */
    static void process_parsed(const bool say_ok=true);

    /**
     * Search M29 command
     */
    FORCE_INLINE static bool is_M29(const char * const cmd) {
      return strstr_P(cmd, PSTR("M29"));
    }

    FORCE_INLINE static void process_stream_char(const char c, uint8_t &sis, char (&buff)[MAX_CMD_SIZE], uint16_t &ind) {

      if (ind >= MAX_CMD_SIZE - 1)
        sis = PS_EOL;                   // Skip the rest on overflow

      if (sis == PS_EOL) return;        // EOL comment or overflow
      else if (sis == PS_PAREN) {       // Inline comment
        if (c == ')') sis = PS_NORMAL;
        return;
      }
      else if (sis >= PS_ESC)           // End escaped char
        sis -= PS_ESC;
      else if (c == '\\') {             // Start escaped char
        sis += PS_ESC;
        if (sis == PS_ESC) return;      // Keep if quoting
      }
      else if (sis == PS_QUOTED) {
        if (c == '"') sis = PS_NORMAL;  // End quoted string
      }
      else if (c == '"')                // Start quoted string
        sis = PS_QUOTED;
      else if (c == ';') {              // Start end-of-line comment
        sis = PS_EOL;
        return;
      }
      else if (c == '(') {              // Start inline comment
        sis = PS_PAREN;
        return;
      }

      buff[ind++] = c;

    }

    FORCE_INLINE static bool process_line_done(uint8_t &sis, char (&buff)[MAX_CMD_SIZE], uint16_t &ind) {
      sis = PS_NORMAL;
      buff[ind] = 0;
      if (ind) { ind = 0; return false; }
      return true;
    }

};

extern Commands commands;

