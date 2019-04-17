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
 */

  #define CODE_M1010
  #define CODE_M1011
  
 /*
  * M1010: Cut fiber
  *
  */
 inline void gcode_M1010(void) {
	 tools.cut_fiber();
 }


/*
 * M1011: Fiber cut Parameters
 *
 *  S<servo-id> A<cut-angle> B<neutral-angle>
 *
 */
inline void gcode_M1011() {
	  if (parser.seen('S')) Tools::cut_servo_id = parser.value_byte();
	  if (parser.seen('A')) Tools::cut_active_angle = parser.value_byte();
	  if (parser.seen('B')) Tools::cut_neutral_angle = parser.value_byte();
}

