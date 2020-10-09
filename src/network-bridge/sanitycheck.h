/*
 * sanitycheck.h
 *
 *  Created on: 9.10.2020
 *      Author: Avazar
 */

#pragma once

#if (NETWORK_BRIDGE_SERIAL<0) || (NETWORK_BRIDGE_SERIAL>1)
  #error "PARAMETER ERROR: NETWORK_BRIDGE_SERIAL can be 0 or 1"
#endif

#if (NETWORK_BRIDGE_SERIAL==1) && (DISABLED(SERIAL_PORT_2) || SERIAL_PORT_2 < -1)
  #error "DEPENDENCY ERROR: NETWORK_BRIDGE_SERIAL can't be 1 if SERIAL_PORT_2 is not enabled."
#endif
