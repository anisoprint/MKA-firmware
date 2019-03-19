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

#if HAS_EEPROM

#define CODE_M1003
#define CODE_M1004
#define CODE_M1005
#define CODE_M1006
#define CODE_M1007
#define CODE_M1008
#define CODE_M1009

/**
 * M1003: Set SN
 */
inline void gcode_M1003(void) {
  strncpy(eeprom.printerSN, parser.string_arg, 16);
  eeprom.printerSN[16] = 0;
  eeprom.Store_Const();
}

/**
 * M1004: Set Printer Version
 */
inline void gcode_M1004(void) {
  strncpy(eeprom.printerVersion, parser.string_arg, 9);
  eeprom.printerVersion[8] = 0;
  eeprom.Store_Const();
}

/**
 * M1005: Print SN
 */
inline void gcode_M1005(void) {
	SERIAL_EMV("SN:", eeprom.printerSN);
}

/**
 * M1006: Print version
 */
inline void gcode_M1006(void) {
	SERIAL_EMV("VER:", eeprom.printerVersion);
}

/**
 * M1007: Print firmware version
 */
inline void gcode_M1007(void) {
	SERIAL_EMV("FWVER:", SHORT_BUILD_VERSION);
}

/**
 * M1008: Print machine name
 */
inline void gcode_M1008(void) {
	SERIAL_EMV("MACHINE:", CUSTOM_MACHINE_NAME);
}

/**
 * M1009: Display firmware update screen
 */
inline void gcode_M1009(void) {
#if ENABLED(NEXTION_HMI)
	NextionHMI::ShowStartScreen(MSG_UPDATING_FIRMWARE, MSG_DO_NOT_TURN_OFF);
#endif
}

#endif
