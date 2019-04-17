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

  #define CODE_M1012
  
 /*
  * M1012: Test switch
  *
  */
 inline void gcode_M1012(void) {
	StateMessage::ActivatePGM(MESSAGE_CRITICAL_ERROR, NEX_ICON_WARNING, "Switch Test", "0", 0, 0, 0, 0, 0);
	#if EXTRUDERS > 1 && HOTENDS > 1

	int switchCount = 0;

	Temperature::tempError = false;

	while (!Temperature::tempError)
	{
		uint8_t new_extruder = 0;
		if (tools.active_extruder == 0) new_extruder = 1;

		if (printer.mode == PRINTER_MODE_FFF) {
			 tools.change(new_extruder);
		}
		switchCount++;
		String str = String(switchCount);
		StateMessage::UpdateMessage(str.c_str());
	}

	sprintf_P(NextionHMI::buffer, "Temperature failure. T=%d. Switch number: %d.", (int)heaters[0].current_temperature, switchCount);
	StateMessage::UpdateMessage(NextionHMI::buffer);

	#endif
	 //tools.cut_fiber();
 }




