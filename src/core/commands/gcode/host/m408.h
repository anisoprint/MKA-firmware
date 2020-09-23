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

#if ENABLED(JSON_OUTPUT)

  #define CODE_M408

  /**
   * M408: JSON STATUS OUTPUT
   * S[uint] - report type
   * P[bool] - autoreport on/off
   */
  inline void gcode_M408(void) {
	uint8_t type = 0;
    if (parser.seen('S')) type = parser.value_byte();
    printer.reportStatusJson(type);

    if (parser.seen('P'))
    {
    	uint8_t interval = parser.value_byte();
    	if (interval>25) interval = 25;
    	if (interval>0)
    	{
    		printer.enableJsonAutoreport(type, interval);
    	}
    	else
    	{
    		printer.disableJsonAutoreport();
    	}

    }

  }

#endif // ENABLED(JSON_OUTPUT)
