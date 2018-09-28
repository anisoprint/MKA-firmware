/*
 * PrintPause.h
 *
 *  Created on: 28 ????. 2018 ?.
 *      Author: Azarov
 */

#ifndef SRC_LCD_NEXTION_HMI_PRINTPAUSE_H_
#define SRC_LCD_NEXTION_HMI_PRINTPAUSE_H_

#if ENABLED(NEXTION_HMI)

namespace PrintPause {

  void DoPauseExtruderMove(AxisEnum axis, const float &length, const float fr);

  bool PausePrint(const float &retract, const point_t &park_point);
  void ResumePrint(const float &load_length=0, const float &purge_length=PAUSE_PARK_EXTRUDE_LENGTH, const int8_t max_beep_count=0);

}


#endif





#endif /* SRC_LCD_NEXTION_HMI_PRINTPAUSE_H_ */
