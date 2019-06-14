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

	static float return_position[2] = {0, 0};
	stepper.synchronize();
	if (parser.seen('R'))
	{
		#if ENABLED(EG6_EXTRUDER)
			float x_target, y_target;
			uint8_t next_extruder = 0;
			if (tools.active_extruder==0) next_extruder = 1;
			if (tools.hotend_switch_path[next_extruder][0].Speed>0)
			{
				mechanics.do_blocking_move_to_xy(
						return_position[X_AXIS],
						return_position[Y_AXIS],
						tools.hotend_switch_path[next_extruder][0].Speed);
			}
		#endif
	}
	else
	{
		return_position[X_AXIS] = mechanics.current_position[X_AXIS];
		return_position[Y_AXIS] = mechanics.current_position[Y_AXIS];

		#if ENABLED(EG6_EXTRUDER)
			float x_target, y_target;
			uint8_t next_extruder = 0;
			if (tools.active_extruder==0) next_extruder = 1;
			if (tools.hotend_switch_path[next_extruder][0].Speed>0)
			{
				mechanics.do_blocking_move_to_xy(
						Mechanics::homeCS2toolCS(tools.active_extruder, tools.hotend_switch_path[next_extruder][0].X, AxisEnum::X_AXIS),
						Mechanics::homeCS2toolCS(tools.active_extruder, tools.hotend_switch_path[next_extruder][0].Y, AxisEnum::Y_AXIS),
						tools.hotend_switch_path[next_extruder][0].Speed);
			}
		#endif


	}

 }




