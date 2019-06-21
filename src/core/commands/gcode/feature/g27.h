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
 * gcode.h
 *
 * Copyright (C) 2017 Alberto Cotronei @MagoKimbra
 */

#if ENABLED(NOZZLE_PARK_FEATURE)

  #define CODE_G27

  /**
   *
   * G27: Park the nozzle
   * G27 X Y Z - Set park point
   * G27 Px - Park
   * P0: If current Z-pos is lower than Z-park then the nozzle will be raised to reach Z-park height
   * P1: No matter the current Z-pos, the nozzle will be raised/lowered to reach Z-park height
   * P2: The nozzle height will be raised by Z-park amount but never going over the machine’s limit of Z_MAX_POS
   * G27 R - Return from parking to position
   */
  inline void gcode_G27(void) {
    // Don't allow nozzle parking without homing first
    if (mechanics.axis_unhomed_error()) { return; }

    Nozzle::park(parser.ushortval('P'));
  }

#endif // NOZZLE_PARK_FEATURE
