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

#if ENABLED(EG6_EXTRUDER)

  #define CODE_M217
  
  /*
   * M217: Toolchange Parameters
   *
   *  A<x_offset> B<y_offset> T<target_tool> S<step (0-11) X<x_pos> Y<y_pos> V<speed mm/s> K<switch movement 0/1>
   *
   */
  inline void gcode_M217() {
	  if (parser.seen('A')) Tools::switch_offset_x = parser.value_linear_units();
	  if (parser.seen('B')) Tools::switch_offset_y = parser.value_linear_units();

	  GET_TARGET_EXTRUDER(217);
	  if (parser.seen('S'))
	  {
		  uint8_t step = parser.value_byte();
		  if (step>=CHANGE_MOVES)
		  {
			  return;
		  }
		  if (parser.seen('X')) Tools::hotend_switch_path[TARGET_EXTRUDER][step].X = parser.value_linear_units();
		  if (parser.seen('Y')) Tools::hotend_switch_path[TARGET_EXTRUDER][step].Y = parser.value_linear_units();
		  if (parser.seen('V')) Tools::hotend_switch_path[TARGET_EXTRUDER][step].Speed = parser.value_linear_units();
		  if (parser.seen('K')) Tools::hotend_switch_path[TARGET_EXTRUDER][step].SwitchMove = parser.value_bool();

	  }

  }


#endif // EG6_EXTRUDER
