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

#define CODE_M81

/**
 * M81: Turn off Power, including Power Supply, if there is one.
 *
 *      This code should ALWAYS be available for EMERGENCY SHUTDOWN!
 */
inline void gcode_M81(void) {
  thermalManager.disable_all_heaters();
  printer.clean_after_print();
  stepper.finish_and_disable();
  printer.setStatus(Halted);

  #if FAN_COUNT > 0
    LOOP_FAN() {
      fans[f].setSpeed(0);
      fans[f].paused_Speed = 0;
      fans[f].setIdle(false);
    }
  #endif

  #if ENABLED(LASER)
    laser.extinguish();
    #if ENABLED(LASER_PERIPHERALS)
      laser.peripherals_off();
    #endif
  #endif

  #if ENABLED(CNCROUTER)
    cnc.disable_router();
  #endif

  printer.safe_delay(1000); // Wait 1 second before switching off

  #if HAS_SUICIDE
    stepper.synchronize();
    printer.suicide();
  #elif HAS_POWER_SWITCH
    powerManager.power_off();
  #endif

	#if ENABLED(NEXTION_HMI)
		NextionHMI::RaiseEvent(HMIevent::SWITCHED_OFF, 0, PSTR(MACHINE_NAME " " MSG_OFF));
	#else
	  LCD_MESSAGEPGM(MACHINE_NAME " " MSG_OFF ".");
	#endif

}
