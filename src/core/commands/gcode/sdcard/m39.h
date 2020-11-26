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
 * Copyright (C) 2020 Avazar
 */

#if HAS_SD_SUPPORT

  #define CODE_M39

  /**
   * M39: Set SD card parameters
   *
   *   P<index>  SD card slot index, if more than one card
   *   U<int>    SPI SS Pin
   *   D<int>    SPI frequency divisor
   *   C<int>	 SD Detect Pin
   *
   */
  inline void gcode_M39(void) {
	  int8_t slot = parser.seen('P') ? parser.value_int() : 0; // selected sd card
	  if (!commands.get_target_sdcard(slot)) return;

	  if (parser.seen('U')) {
	     pin_t new_ss_pin = parser.value_pin();
	     if (new_ss_pin != sdStorage.pins[slot])
	     {
	     	sdStorage.pins[slot] = new_ss_pin;
            SERIAL_LM(ECHO, MSG_CHANGE_PIN);
	     }
	  }

	  if (parser.seen('D')) {
	     pin_t new_ss_div = parser.value_byte();
	     if (new_ss_div != sdStorage.spi_speed_divisors[slot])
	     {
	     	sdStorage.spi_speed_divisors[slot] = new_ss_div;
	     }
	  }

	  if (parser.seen('C')) {
	     pin_t new_detect_pin = parser.value_pin();
	     if (new_detect_pin != sdStorage.detect_pins[slot])
	     {
	     	sdStorage.detect_pins[slot] = new_detect_pin;
            SERIAL_LM(ECHO, MSG_CHANGE_PIN);
	     }
	  }



  }


#endif // HAS_SD_SUPPORT
