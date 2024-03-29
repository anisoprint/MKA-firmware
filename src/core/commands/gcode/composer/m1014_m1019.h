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

  #define CODE_M1014
  #define CODE_M1015
  #define CODE_M1016
  #define CODE_M1017
  #define CODE_M1018
  #define CODE_M1019
  
 /*
  * M1014: Set SBC status S1 - connected, S0 - temporarily disconnected
  *
  */
 inline void gcode_M1014(void) {
	 NetBridgeStatus status = Connected;
	 if (parser.seen('S')) status = parser.value_byte() == 0 ? TempDisconnected : Connected;
	 netBridgeManager.UpdateNetBridgeStatus(status);
 }


/*
 * M1015: Set network status
 *
 *  E[0/1] - ethernet, I[0/1] - wifi enabled, W[0/1] wifi connected, A[0/1] - Aura connect
 *
 */
inline void gcode_M1015() {
	  if (parser.seen('E')) netBridgeManager.UpdateEthernetConnected(parser.value_bool());
	  if (parser.seen('I')) netBridgeManager.UpdateWifiEnabled(parser.value_bool());
	  if (parser.seen('W')) netBridgeManager.UpdateWifiConnected(parser.value_bool());
	  if (parser.seen('A')) netBridgeManager.UpdateAcConnected(parser.value_bool());
}

/*
 * M1016: Send command to network bridge
 *
 */
inline void gcode_M1016() {
	char buffer[512] = {0};
	if (netBridgeManager.SendCommand(parser.string_arg, buffer, 512, NETWORK_BRIDGE_TIMEOUT))
	{
		SERIAL_ET(buffer);
	}
	else
	{
		SERIAL_LM(ER, "Can't send command");
	}
}

/*
 * M1017: Set job awaiting flag
 *
 */
inline void gcode_M1017() {
	  if (parser.seen('S')) netBridgeManager.UpdateJobAwaiting(parser.value_bool());
}

/*
 * M1018: Clear AC dir in internal SD storage
 *
 */
inline void gcode_M1018() {
      auto type = parser.intval('S', 0);
      switch (type)
      {
      	  case 0:
      		  SERIAL_EMT(INFO, PSTR("Test info"));
      		  break;
      	  case 1:
      		  SERIAL_EMT(WARNING, PSTR("Test warning"));
      		  break;
      	  case 2:
      		  SERIAL_EMT(ER, PSTR("Test error"));
      		  break;
      }

}

/*
 * M1019: Show progress while receiving file from server
 * M1019 P<progress> <filename>
 *
 */
inline void gcode_M1019() {
	if (parser.seen('P'))
	{
		float percent = parser.value_float();
		netBridgeManager.ShowFileReceiveProgress(parser.string_arg, percent);
	}


}



