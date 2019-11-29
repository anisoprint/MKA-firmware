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

#define CODE_M569

/**
 * M569: Stepper driver control
 *
 *  E[bool]           - Set X direction
 *  Y[bool]           - Set Y direction
 *  Z[bool]           - Set Z direction
 *  E[bool]  		  - Set Extruder direction
 *  U[bool]  		  - Set Extruder direction
 *  V[bool]  		  - Set Extruder direction
 *
 */

inline void gcode_M569() {

  LOOP_XYZE(i) {
    if (parser.seen(axis_codes[i])) {
    	stepper.stepper_dir_invert[i] = parser.value_bool();
    }
  }

  stepper.set_directions();

}
