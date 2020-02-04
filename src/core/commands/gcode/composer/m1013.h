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

  #define CODE_M1013
  
 /*
  *
  * M1013: Park/Unpark nozzle for heating near wipe station
  * M1013 - Park
  * M1013 R - Return
  *
  */



 inline void gcode_M1013(void) {
	#if ENABLED(EG6_EXTRUDER)
		if (parser.seen('R') )
		{
			tools.unpark_from_wipe();
		}
		else
		{
			tools.park_to_wipe();
		}
	#endif
 }




