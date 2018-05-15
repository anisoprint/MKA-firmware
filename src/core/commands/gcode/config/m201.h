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

#define CODE_M201

/**
 * M201: Set max acceleration in units/s^2 for print moves (M201 X1000 Y1000)
 *
 */
inline void gcode_M201(void) {

  LOOP_XYZE(i) {
    if (parser.seen(axis_codes[i])) {
      #if MECH(DELTA)
        const float value = parser.value_per_axis_unit((AxisEnum)a);
        if (i >= E_AXIS)
          mechanics.max_acceleration_mm_per_s2[i] = value;
        else
          LOOP_XYZ(axis) mechanics.max_acceleration_mm_per_s2[axis] = value;
      #else
        mechanics.max_acceleration_mm_per_s2[i] = parser.value_axis_units((AxisEnum)i);
      #endif
    }
  }
  // steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
  mechanics.reset_acceleration_rates();
}
