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

#if ENABLED(NEXTION)

  #if HAS_SDSUPPORT

    #define CODE_M35

    /**
     * M35: Upload Firmware to Nextion from SD
     */
    inline void gcode_M35(void) {
      UploadNewFirmware();
    }
  #endif
#elif ENABLED(NEXTION_HMI)


  #if HAS_SDSUPPORT

    #define CODE_M35

    /**
     * M35: Upload Firmware to Nextion from SD or serial
     */
    inline void gcode_M35(void) {

      if (parser.seen('S')) {

    	  if (Commands::current_command_port < 0 || Commands::current_command_port >= NUM_SERIAL)
    	  {
    		  SERIAL_LM(ER, MSG_ERR_M35_SERIAL);
    		  return;
    	  }

    	  uint32_t tftSize = parser.value_ulong();
    	  NextionHMI::UploadFirmwareFromSerial(tftSize, Commands::current_command_port);
      }
      else
      {
    	  NextionHMI::UploadFirmwareFromSD();
      }
    }
  #endif

#endif // ENABLED(NEXTION)
