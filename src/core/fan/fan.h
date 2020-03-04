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
 * fan.h
 *
 * Copyright (C) 2017 Alberto Cotronei @MagoKimbra
 */

#ifndef _FAN_H_
#define _FAN_H_

#if FAN_COUNT > 0

  enum FlagFans {
    fan_flag_hardware_inverted,
    fan_flag_idle
  };

  class Fan {

    public: /** Constructor */

      Fan() {}

    public: /** Public Parameters */

      pin_t     pin;
      uint8_t   Speed,
	  	  	  	speed_nocorr,
                min_Speed,
                paused_Speed,
                Kickstart,
                pwm_pos,
                FanFlag;
      int8_t    speed_correction;
      int8_t    autoMonitored;
      uint16_t  freq,
                triggerTemperatures;
      int16_t   lastpwm;

    public: /** Public Function */

      void init();
      void SetAutoMonitored(const int8_t h);
      void spin();

      FORCE_INLINE void setHWInverted(const bool onoff) {
        SET_BIT(FanFlag, fan_flag_hardware_inverted, onoff);
      }
      FORCE_INLINE bool isHWInverted() { return TEST(FanFlag, fan_flag_hardware_inverted); }

      FORCE_INLINE void setIdle(const bool onoff) {
        if (onoff != isIdle()) {
          SET_BIT(FanFlag, fan_flag_idle, onoff);
          if (onoff) {
            paused_Speed = Speed;
            setSpeed(0);
          }
          else
          {
        	setSpeed(paused_Speed);
          }

        }
      }
      FORCE_INLINE bool isIdle() { return TEST(FanFlag, fan_flag_idle); }

      FORCE_INLINE void setSpeed(uint8_t new_speed)
      {
    	  speed_nocorr = new_speed;
    	  if (new_speed!=0 && speed_correction!=0)
    	  {
    		  int16_t speed_after_correction = new_speed + speed_correction*255/100;
    		  NOMORE(speed_after_correction, 255);
    		  NOLESS(speed_after_correction, 0);
    		  new_speed = (uint8_t)speed_after_correction;
    	  }
    	  Speed = new_speed;
      }

      FORCE_INLINE void updateCorrection() {
    	if (speed_nocorr !=0 )
    	{
    		setSpeed(speed_nocorr);
    	}
      }

      #if HARDWARE_PWM
        void SetHardwarePwm();
      #endif

  };

  extern Fan fans[FAN_COUNT];

#endif // FAN_COUNT > 0

#endif /* _FAN_H_ */
