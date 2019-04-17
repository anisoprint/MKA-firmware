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

  #define CODE_M704

  
  /*
   * M704: Load/unload filament config
   *
   *  E[extruder] - Extruder driver number.
   *  L[distance] - Load distance
   *  U[distance] - Unload distance
   *
   *  A[val] - PAUSE_PARK_RETRACT_FEEDRATE (mm/s) Initial retract feedrate.
   *  B[val] - PAUSE_PARK_RETRACT_LENGTH (mm) Initial retract. This short retract is done immediately, before parking the nozzle.
   *  C[val] - PAUSE_PARK_UNLOAD_FEEDRATE (mm/s) Unload filament feedrate. This can be pretty fast.
   *  D[val] - PAUSE_PARK_LOAD_FEEDRATE (mm/s) Load filament feedrate. This can be pretty fast.
   *  K[val] - PAUSE_PARK_EXTRUDE_FEEDRATE (mm/s) Extrude feedrate (after loading). Should be slower than load feedrate.
   *
   */
  inline void gcode_M704(void) {
	  if (parser.seenval('E'))
	  {
		  int8_t e =  parser.value_int();
		  if (parser.seen('L')) PrintPause::LoadDistance[e] = parser.value_linear_units();
		  if (parser.seen('U')) PrintPause::UnloadDistance[e] = parser.value_linear_units();
	  }
	  if (parser.seen('A')) PrintPause::RetractDistance = parser.value_linear_units();
	  if (parser.seen('B')) PrintPause::RetractFeedrate = parser.value_linear_units();
	  if (parser.seen('C')) PrintPause::LoadFeedrate = parser.value_linear_units();
	  if (parser.seen('D')) PrintPause::UnloadFeedrate = parser.value_linear_units();
	  if (parser.seen('K')) PrintPause::ExtrudeFeedrate = parser.value_linear_units();

  }

