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

#ifndef ULTRALCD_IMPL_HD44780_H
#define ULTRALCD_IMPL_HD44780_H

/**
 * Implementation of the LCD display routines for a Hitachi HD44780 display.
 * These are the most common LCD character displays.
 */

#include "ultralcd_common_HD44780.h"

////////////////////////////////////
// Create LCD class instance and chipset-specific information
#if ENABLED(LCD_I2C_TYPE_PCF8575)
  LCD_CLASS lcd(LCD_I2C_ADDRESS, LCD_I2C_PIN_EN, LCD_I2C_PIN_RW, LCD_I2C_PIN_RS, LCD_I2C_PIN_D4, LCD_I2C_PIN_D5, LCD_I2C_PIN_D6, LCD_I2C_PIN_D7);

#elif ENABLED(LCD_I2C_TYPE_MCP23017)
  #if ENABLED(DETECT_DEVICE)
    LCD_CLASS lcd(LCD_I2C_ADDRESS, 1);
  #else
    LCD_CLASS lcd(LCD_I2C_ADDRESS);
  #endif

#elif ENABLED(LCD_I2C_TYPE_MCP23008)
  #if ENABLED(DETECT_DEVICE)
    LCD_CLASS lcd(LCD_I2C_ADDRESS, 1);
  #else
    LCD_CLASS lcd(LCD_I2C_ADDRESS);
  #endif

#elif ENABLED(LCD_I2C_TYPE_PCA8574)
  LCD_CLASS lcd(LCD_I2C_ADDRESS, LCD_WIDTH, LCD_HEIGHT);

// 2 wire Non-latching LCD SR from:
// https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/schematics#!shiftregister-connection
#elif ENABLED(SR_LCD_2W_NL)
  #if PIN_EXISTS(SR_STROBE)
    LCD_CLASS lcd(SR_DATA_PIN, SR_CLK_PIN, SR_STROBE_PIN);
  #else
    LCD_CLASS lcd(SR_DATA_PIN, SR_CLK_PIN);
  #endif

#elif ENABLED(LCM1602)
  LCD_CLASS lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#else
  // Standard directly connected LCD implementations
  LCD_CLASS lcd(LCD_PINS_RS, LCD_PINS_ENABLE, LCD_PINS_D4, LCD_PINS_D5, LCD_PINS_D6, LCD_PINS_D7); //RS,Enable,D4,D5,D6,D7
#endif

#include "fontutils.h"
#include "lcdprint.h"

#if ENABLED(LCD_PROGRESS_BAR)
  static millis_t progress_bar_ms = 0;     // Start millis of the current progress bar cycle
  #if PROGRESS_MSG_EXPIRE > 0
    static millis_t expire_status_ms = 0;  // millis at which to expire the status message
  #endif
#endif

#if ENABLED(LCD_HAS_STATUS_INDICATORS)
  static void lcd_implementation_update_indicators();
#endif

static void createChar_P(const char c, const byte * const ptr) {
  byte temp[8];
  for (uint8_t i = 0; i < 8; i++)
    temp[i] = pgm_read_byte(&ptr[i]);
  lcd.createChar(c, temp);
}

static void lcd_set_custom_characters(
  #if ENABLED(LCD_PROGRESS_BAR) || ENABLED(SHOW_BOOTSCREEN)
    const uint8_t screen_charset=CHARSET_INFO
  #endif
) {
  // CHARSET_BOOT
  #if ENABLED(SHOW_BOOTSCREEN)
    const static PROGMEM byte corner[4][8] = { {
      B00000,
      B00000,
      B00000,
      B00000,
      B00001,
      B00010,
      B00100,
      B00100
    }, {
      B00000,
      B00000,
      B00000,
      B11100,
      B11100,
      B01100,
      B00100,
      B00100
    }, {
      B00100,
      B00010,
      B00001,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000
    }, {
      B00100,
      B01000,
      B10000,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000
    } };
  #endif // SHOW_BOOTSCREEN

  // CHARSET_INFO
  const static PROGMEM byte bedTemp[8] = {
    B00000,
    B11111,
    B10101,
    B10001,
    B10101,
    B11111,
    B00000,
    B00000
  };

  const static PROGMEM byte degree[8] = {
    B01100,
    B10010,
    B10010,
    B01100,
    B00000,
    B00000,
    B00000,
    B00000
  };

  const static PROGMEM byte thermometer[8] = {
    B00100,
    B01010,
    B01010,
    B01010,
    B01010,
    B11111,
    B11111,
    B01110
  };

  const static PROGMEM byte Humidity[8] = {
    B00100,
    B00100,
    B01010,
    B10001,
    B10001,
    B01110,
    B00000,
    B00000
  };

  const static PROGMEM byte uplevel[8] = {
    B00100,
    B01110,
    B11111,
    B00100,
    B11100,
    B00000,
    B00000,
    B00000
  };

  const static PROGMEM byte feedrate[8] = {
    B11100,
    B10000,
    B11000,
    B10111,
    B00101,
    B00110,
    B00101,
    B00000
  };

  const static PROGMEM byte time_clock[8] = {
    B00000,
    B01110,
    B10011,
    B10101,
    B10001,
    B01110,
    B00000,
    B00000
  };

  #if ENABLED(LCD_PROGRESS_BAR)

    // CHARSET_INFO
    const static PROGMEM byte progress[3][8] = { {
      B00000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B00000
    }, {
      B00000,
      B10100,
      B10100,
      B10100,
      B10100,
      B10100,
      B10100,
      B00000
    }, {
      B00000,
      B10101,
      B10101,
      B10101,
      B10101,
      B10101,
      B10101,
      B00000
    } };

  #endif // LCD_PROGRESS_BAR

  #if HAS_SD_SUPPORT

    // CHARSET_MENU
    const static PROGMEM byte refresh[8] = {
      B00000,
      B00110,
      B11001,
      B11000,
      B00011,
      B10011,
      B01100,
      B00000,
    };
    const static PROGMEM byte folder[8] = {
      B00000,
      B11100,
      B11111,
      B10001,
      B10001,
      B11111,
      B00000,
      B00000
    };

  #endif // SDSUPPORT

  #if ENABLED(SHOW_BOOTSCREEN)
    // Set boot screen corner characters
    if (screen_charset == CHARSET_BOOT) {
      for (uint8_t i = 4; i--;)
        createChar_P(i, corner[i]);
    }
    else
  #endif
    { // Info Screen uses 5 special characters
      #if ENABLED(DHT_SENSOR)
        createChar_P(LCD_BEDTEMP_CHAR, Humidity);
      #else
        createChar_P(LCD_BEDTEMP_CHAR, bedTemp);
      #endif
      createChar_P(LCD_DEGREE_CHAR, degree);
      createChar_P(LCD_STR_THERMOMETER[0], thermometer);
      createChar_P(LCD_FEEDRATE_CHAR, feedrate);
      createChar_P(LCD_CLOCK_CHAR, time_clock);

      #if ENABLED(LCD_PROGRESS_BAR)
        if (screen_charset == CHARSET_INFO) { // 3 Progress bar characters for info screen
          for (int16_t i = 3; i--;)
            createChar_P(LCD_STR_PROGRESS[i], progress[i]);
        }
        else
      #endif
        {
          createChar_P(LCD_UPLEVEL_CHAR, uplevel);
          #if ENABLED(SDSUPPORT)
            // SD Card sub-menu special characters
            createChar_P(LCD_STR_REFRESH[0], refresh);
            createChar_P(LCD_STR_FOLDER[0], folder);
          #endif
        }
    }

}

static void lcd_implementation_init(
  #if ENABLED(LCD_PROGRESS_BAR)
    const uint8_t screen_charset=CHARSET_INFO
  #endif
) {

  #if ENABLED(LCD_I2C_TYPE_PCF8575)
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
    #ifdef LCD_I2C_PIN_BL
      lcd.setBacklightPin(LCD_I2C_PIN_BL, POSITIVE);
      lcd.setBacklight(HIGH);
    #endif

  #elif ENABLED(LCD_I2C_TYPE_MCP23017)
    lcd.setMCPType(LTI_TYPE_MCP23017);
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
    lcd_implementation_update_indicators();

  #elif ENABLED(LCD_I2C_TYPE_MCP23008)
    lcd.setMCPType(LTI_TYPE_MCP23008);
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);

  #elif ENABLED(LCD_I2C_TYPE_PCA8574)
    lcd.init();
    lcd.backlight();

  #else
    #if (LCD_PINS_RS != -1) && (LCD_PINS_ENABLE != -1)
      // required for RAMPS-FD, but does no harm for other targets
      SET_OUTPUT(LCD_PINS_RS);
      SET_OUTPUT(LCD_PINS_ENABLE);
    #endif
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  #endif

  lcd_set_custom_characters(
    #if ENABLED(LCD_PROGRESS_BAR)
      screen_charset
    #endif
  );

  lcd.clear();
}

void lcd_implementation_clear() { lcd.clear(); }

#if ENABLED(SHOW_BOOTSCREEN)

  void lcd_erase_line(const int16_t line) {
    lcd_moveto(0, line);
    for (uint8_t i = LCD_WIDTH + 1; --i;)
      lcd_put_wchar(' ');
  }

  // Scroll the PSTR 'text' in a 'len' wide field for 'time' milliseconds at position col,line
  void lcd_scroll(const int16_t col, const int16_t line, const char* const text, const int16_t len, const int16_t time) {
    #if 1
      lcd_put_u8str(text);
    #else
      char tmp[LCD_WIDTH + 1] = {0};
      int16_t n = max(utf8_strlen_P(text) - len, 0);
      for (int16_t i = 0; i <= n; i++) {
        utf8_strncpy_p(tmp, text + i, min(len, LCD_WIDTH));
        lcd_moveto(col, line);
        lcd_put_u8str(tmp);
        delay(time / max(n, 1));
      }
    #endif
  }

  static void logo_lines(const char* const extra) {
    int16_t indent = (LCD_WIDTH - 8 - utf8_strlen_P(extra)) / 2;
    lcd_moveto(indent, 0); lcd_put_wchar('\x00'); lcd_put_u8str_rom(PSTR( "------" ));  lcd_put_wchar('\x01');
    lcd_moveto(indent, 1);                        lcd_put_u8str_rom(PSTR("|MK4duo|"));  lcd_put_u8str_rom(extra);
    lcd_moveto(indent, 2); lcd_put_wchar('\x02'); lcd_put_u8str_rom(PSTR( "------" ));  lcd_put_wchar('\x03');
  }

  void lcd_bootscreen() {
    lcd_set_custom_characters(CHARSET_BOOT);
    lcd.clear();

    #define LCD_EXTRA_SPACE (LCD_WIDTH-8)

    #define CENTER_OR_SCROLL(STRING,DELAY) \
      lcd_erase_line(3); \
      if (utf8_strlen(STRING) <= LCD_WIDTH) { \
        lcd_moveto((LCD_WIDTH - utf8_strlen_P(PSTR(STRING))) / 2, 3); \
        lcd_put_u8str_rom(PSTR(STRING)); \
        HAL::delayMilliseconds(DELAY); \
      } \
      else { \
        lcd_scroll(0, 3, PSTR(STRING), LCD_WIDTH, DELAY); \
      }

    #if ENABLED(STRING_SPLASH_LINE1)
      //
      // Show the MK4duo logo with splash line 1
      //
      if (LCD_EXTRA_SPACE >= utf8_strlen(STRING_SPLASH_LINE1) + 1) {
        //
        // Show the MK4duo logo, splash line1, and splash line 2
        //
        logo_lines(PSTR(STRING_SPLASH_LINE1));
        #if ENABLED(STRING_SPLASH_LINE2)
          CENTER_OR_SCROLL(STRING_SPLASH_LINE2, BOOTSCREEN_TIMEOUT);
        #else
          HAL::delayMilliseconds(BOOTSCREEN_TIMEOUT);
        #endif
      }
      else {
        //
        // Show the MK4duo logo with splash line 1
        // After a delay show splash line 2, if it exists
        //
        #if ENABLED(STRING_SPLASH_LINE2)
          #define _SPLASH_WAIT_1 (BOOTSCREEN_TIMEOUT - 500)
        #else
          #define _SPLASH_WAIT_1 BOOTSCREEN_TIMEOUT
        #endif
        logo_lines(PSTR(""));
        CENTER_OR_SCROLL(STRING_SPLASH_LINE1, _SPLASH_WAIT_1);
        #if ENABLED(STRING_SPLASH_LINE2)
          CENTER_OR_SCROLL(STRING_SPLASH_LINE2, _SPLASH_WAIT_1);
        #endif
      }
    #elif ENABLED(STRING_SPLASH_LINE2)
      //
      // Show splash line 2 only, alongside the logo if possible
      //
      if (LCD_EXTRA_SPACE >= utf8_strlen(STRING_SPLASH_LINE2) + 1) {
        logo_lines(PSTR(" " STRING_SPLASH_LINE2));
        HAL::delayMilliseconds(BOOTSCREEN_TIMEOUT);
      }
      else {
        logo_lines(PSTR(""));
        CENTER_OR_SCROLL(STRING_SPLASH_LINE2, BOOTSCREEN_TIMEOUT);
      }
    #else
      //
      // Show only the MK4duo logo
      //
      logo_lines(PSTR(""));
      HAL::delayMilliseconds(BOOTSCREEN_TIMEOUT);
    #endif

    lcd.clear();
    HAL::delayMilliseconds(100);
    lcd_set_custom_characters();
    lcd.clear();
  }

#endif // SHOW_BOOTSCREEN

void lcd_kill_screen() {
  lcd_moveto(0, 0);
  lcd_put_u8str(lcd_status_message);
  #if LCD_HEIGHT < 4
    lcd_moveto(0, 2);
  #else
    lcd_moveto(0, 2);
    lcd_put_u8str_rom(PSTR(MSG_HALTED));
    lcd_moveto(0, 3);
  #endif
  lcd_put_u8str_rom(PSTR(MSG_PLEASE_RESET));
}

FORCE_INLINE void _draw_axis_label(const AxisEnum axis, const char* const pstr, const bool blink) {
  if (blink)
    lcd_put_u8str_rom(pstr);
  else {
    if (!printer.isAxisHomed(axis))
      lcd_put_wchar('?');
    else
      lcd_put_u8str_rom(pstr);
  }
}

#if ENABLED(DHT_SENSOR)
  FORCE_INLINE void _draw_humidity_status() {
    lcd_put_u8str((char)LCD_BEDTEMP_CHAR);
    lcd_put_u8str(itostr3(dhtsensor.Humidity));
  }
#endif

FORCE_INLINE void _draw_heater_status(const uint8_t heater, const char prefix, const bool blink) {

  const float t1 = (heaters[heater].current_temperature),
              t2 = (heaters[heater].target_temperature);

  if (prefix >= 0) lcd_put_wchar(prefix);

  lcd_put_u8str(itostr3(t1 + 0.5));
  lcd_put_wchar('/');

  #if !HEATER_IDLE_HANDLER
    UNUSED(blink);
  #else
    const bool isIdle = heaters[heater].isIdle();

    if (!blink && isIdle) {
      lcd_put_wchar(' ');
      if (t2 >= 10) lcd_put_wchar(' ');
      if (t2 >= 100) lcd_put_wchar(' ');
    }
    else
  #endif
      lcd_put_u8str(itostr3left(t2 + 0.5));

  if (prefix >= 0) {
    lcd_put_wchar(LCD_DEGREE_CHAR);
    lcd_put_wchar(' ');
    if (t2 < 10) lcd_put_wchar(' ');
  }
}

#if ENABLED(LCD_PROGRESS_BAR)

  inline void lcd_draw_progress_bar(const uint8_t percent) {
    const int16_t tix = (int16_t)(percent * (LCD_WIDTH) * 3) / 100,
                  cel = tix / 3,
                  rem = tix % 3;
    uint8_t i = LCD_WIDTH;
    char msg[LCD_WIDTH + 1], b = ' ';
    msg[LCD_WIDTH] = '\0';
    while (i--) {
      if (i == cel - 1)
        b = LCD_STR_PROGRESS[2];
      else if (i == cel && rem != 0)
        b = LCD_STR_PROGRESS[rem - 1];
      msg[i] = b;
    }
    lcd_put_u8str(msg);
  }

#endif // LCD_PROGRESS_BAR

/**
Possible status screens:
16x2   |000/000 B000/000|
       |0123456789012345|

16x4   |000/000 B000/000|
       |SD100%  Z 000.00|
       |F100%     T--:--|
       |0123456789012345|

20x2   |T000/000D B000/000D |
       |01234567890123456789|

20x4   |T000/000D B000/000D |
       |X 000 Y 000 Z 000.00|
       |F100%  SD100% T--:--|
       |01234567890123456789|

20x4   |T000/000D B000/000D |
       |T000/000D   Z 000.00|
       |F100%  SD100% T--:--|
       |01234567890123456789|
*/
static void lcd_implementation_status_screen() {
  const bool blink = lcd_blink();

  //
  // Line 1
  //

  lcd_moveto(0, 0);

  #if LCD_WIDTH < 20

    //
    // Hotend 0 Temperature
    //
    #if HAS_TEMP_HOTEND
      _draw_heater_status(0, -1, blink);
    #endif

    //
    // Hotend 1 or Bed Temperature
    //
    #if HOTENDS > 1 || HAS_TEMP_BED

      lcd_moveto(8, 0);
      #if HOTENDS > 1
        lcd_put_wchar((char)LCD_STR_THERMOMETER[0]);
        _draw_heater_status(1, -1, blink);
      #else
        lcd_put_wchar((char)LCD_BEDTEMP_CHAR);
        _draw_heater_status(BED_INDEX, -1, blink);
      #endif

    #endif // HOTENDS > 1 || HAS_TEMP_BED

  #else // LCD_WIDTH >= 20

    //
    // Hotend 0 Temperature
    //
    #if HAS_TEMP_HOTEND
      _draw_heater_status(0, LCD_STR_THERMOMETER[0], blink);
    #endif

    //
    // Hotend 1 or Bed Temperature
    //
    #if HOTENDS > 1 || HAS_TEMP_BED || ENABLED(DHT_SENSOR)
      lcd_moveto(10, 0);
      #if HOTENDS > 1
        _draw_heater_status(1, LCD_STR_THERMOMETER[0], blink);
      #elif HAS_TEMP_BED
        _draw_heater_status(BED_INDEX, (
          #if HAS_LEVELING
            bedlevel.leveling_active && blink ? '_' :
          #endif
          LCD_BEDTEMP_CHAR
        ), blink);
      #else
        _draw_humidity_status();
      #endif

    #endif // HOTENDS > 1 || HAS_TEMP_BED

  #endif // LCD_WIDTH >= 20

  //
  // Line 2
  //

  #if LCD_HEIGHT > 2

    #if LCD_WIDTH < 20

      #if HAS_SD_SUPPORT
        lcd_moveto(0, 2);
        lcd_put_u8str_rom(PSTR("SD"));
        if (IS_SD_PRINTING)
          lcd_put_u8str(itostr3(printer.progress));
        else
          lcd_put_u8str_rom(PSTR("---"));
          lcd_put_wchar('%');
      #endif // SDSUPPORT

    #else // LCD_WIDTH >= 20

      lcd_moveto(0, 1);

      // If the first line has two extruder temps,
      // show more temperatures on the next line

      #if HOTENDS > 2 || (HOTENDS > 1 && HAS_TEMP_BED)

        #if HOTENDS > 2
          _draw_heater_status(2, LCD_STR_THERMOMETER[0], blink);
          lcd_moveto(10, 1);
        #endif

        _draw_heater_status(BED_INDEX, (
          #if HAS_LEVELING
            bedlevel.leveling_active && blink ? '_' :
          #endif
          LCD_BEDTEMP_CHAR
        ), blink);

      #else // HOTENDS <= 2 && (HOTENDS <= 1 || !HAS_TEMP_BED)
        // Before homing the axis letters are blinking 'X' <-> '?'.
        // When axis is homed but axis known position is false the axis letters are blinking 'X' <-> ' '.
        // When everything is ok you see a constant 'X'.

        _draw_axis_label(X_AXIS, PSTR(MSG_X), blink);
        lcd_put_u8str(ftostr4sign(LOGICAL_X_POSITION(mechanics.current_position[X_AXIS])));

        lcd_put_wchar(' ');

        _draw_axis_label(Y_AXIS, PSTR(MSG_Y), blink);
        lcd_put_u8str(ftostr4sign(LOGICAL_Y_POSITION(mechanics.current_position[Y_AXIS])));

      #endif // HOTENDS <= 2 && (HOTENDS <= 1 || !HAS_TEMP_BED)

    #endif // LCD_WIDTH >= 20

    lcd_moveto(LCD_WIDTH - 8, 1);
    _draw_axis_label(Z_AXIS, PSTR(MSG_Z), blink);
    lcd_put_u8str(ftostr52sp(FIXFLOAT(LOGICAL_Z_POSITION(mechanics.current_position[Z_AXIS]))));

    #if HAS_LEVELING && !HAS_TEMP_BED
      lcd_put_wchar(bedlevel.leveling_active || blink ? '_' : ' ');
    #endif

  #endif // LCD_HEIGHT > 2

  //
  // Line 3
  //

  #if LCD_HEIGHT > 3

    lcd_moveto(0, 2);
    lcd_put_wchar(LCD_FEEDRATE_CHAR);
    lcd_put_u8str(itostr3(mechanics.feedrate_percentage));
    lcd_put_wchar('%');

    #if LCD_WIDTH >= 20 && HAS_SD_SUPPORT

      lcd_moveto(7, 2);
      lcd_put_u8str_rom(PSTR("SD"));
      if (IS_SD_PRINTING)
        lcd_put_u8str(itostr3(printer.progress));
      else
        lcd_put_u8str_rom(PSTR("---"));
      lcd_put_wchar('%');

    #endif // LCD_WIDTH >= 20 && SDSUPPORT

    char buffer[10];
    duration_t elapsed = print_job_counter.duration();
    uint8_t len = elapsed.toDigital(buffer);

    lcd_moveto(LCD_WIDTH - len - 1, 2);
    #if HAS_LCD_POWER_SENSOR
      if (millis() < print_millis + 1000) {
        lcd_put_wchar(LCD_CLOCK_CHAR);
        lcd_put_u8str(buffer);
      }
      else {
        lcd_put_u8str(itostr4(powerManager.consumption_hour - powerManager.startpower));
        lcd_put_wchar('Wh');
      }
    #else
      lcd_put_wchar(LCD_CLOCK_CHAR);
      lcd_put_u8str(buffer);
    #endif

  #endif // LCD_HEIGHT > 3

  //
  // Last Line
  // Status Message (which may be a Progress Bar or Filament display)
  //

  lcd_moveto(0, LCD_HEIGHT - 1);

  #if ENABLED(LCD_PROGRESS_BAR)

    // Draw the progress bar if the message has shown long enough
    // or if there is no message set.
    if (printer.progress && (ELAPSED(millis(), progress_bar_ms + PROGRESS_BAR_MSG_TIME) || !lcd_status_message[0]))
      return lcd_draw_progress_bar(printer.progress);

  #elif (HAS_LCD_FILAMENT_SENSOR && ENABLED(SDSUPPORT)) || HAS_LCD_POWER_SENSOR

    #if HAS_LCD_FILAMENT_SENSOR && HAS_SD_SUPPORT
      // Show Filament Diameter and Volumetric Multiplier % or Power Sensor
      // After allowing lcd_status_message to show for 5 seconds
      if (ELAPSED(millis(), previous_lcd_status_ms + 5000UL)) {
        lcd_put_u8str_rom(PSTR("Dia "));
        lcd_put_u8str(ftostr12ns(filament_width_meas));
        lcd_put_u8str_rom(PSTR(" V"));
        lcd_put_u8str(itostr3(100.0 * (
          printer.isVolumetric()
            ? tools.volumetric_area_nominal / tools.volumetric_multiplier[FILAMENT_SENSOR_EXTRUDER_NUM]
            : tools.volumetric_multiplier[FILAMENT_SENSOR_EXTRUDER_NUM]
        )
      ));
      lcd_put_wchar('%');
      return;
      }
    #endif

    #if HAS_LCD_POWER_SENSOR
      else if (ELAPSED(millis(), previous_lcd_status_ms + 10000UL)) {
        lcd_put_u8str_rom(PSTR("P:"));
        lcd_put_u8str(ftostr43sign(powerManager.consumption_meas));
        lcd_put_u8str_rom(PSTR("W C:"));
        lcd_put_u8str(ltostr7(powerManager.consumption_hour));
        lcd_put_u8str_rom(PSTR("Wh"));
        return;
      }
    #endif

  #endif // FILAMENT_LCD_DISPLAY || POWER_SENSOR

  #if ENABLED(STATUS_MESSAGE_SCROLLING)
    static bool last_blink = false;
    const uint8_t slen = utf8_strlen(lcd_status_message);
    const char *stat = lcd_status_message + status_scroll_pos;
    if (slen <= LCD_WIDTH)
      lcd_put_u8str(stat);                                        // The string isn't scrolling
    else {
      if (status_scroll_pos <= slen - LCD_WIDTH)
        lcd_put_u8str(stat);                                      // The string fills the screen
      else {
        uint8_t chars = LCD_WIDTH;
        if (status_scroll_pos < slen) {                       // First string still visible
          lcd_put_u8str(stat);                                    // The string leaves space
          chars -= slen - status_scroll_pos;                  // Amount of space left
        }
        lcd_put_wchar('.');                                       // Always at 1+ spaces left, draw a dot
        if (--chars) {
          if (status_scroll_pos < slen + 1)                   // Draw a second dot if there's space
            --chars, lcd_put_wchar('.');
          if (chars) lcd_put_u8str_max(lcd_status_message, chars); // Print a second copy of the message
        }
      }
      if (last_blink != blink) {
        last_blink = blink;
        // Skip any non-printing bytes
        if (status_scroll_pos < slen) while (!PRINTABLE(lcd_status_message[status_scroll_pos])) status_scroll_pos++;
        if (++status_scroll_pos >= slen + 2) status_scroll_pos = 0;
      }
    }
  #else
    lcd_put_u8str(lcd_status_message);
  #endif
}

#if ENABLED(ULTIPANEL)

  #if ENABLED(ADVANCED_PAUSE_FEATURE)

    static void lcd_implementation_hotend_status(const uint8_t row, const uint8_t extruder=tools.active_extruder) {
      if (row < LCD_HEIGHT) {
        lcd_moveto(LCD_WIDTH - 9, row);
        _draw_heater_status(extruder, LCD_STR_THERMOMETER[0], lcd_blink());
      }
    }

  #endif // ADVANCED_PAUSE_FEATURE

  static void lcd_implementation_drawmenu_static(const uint8_t row, const char* pstr, const bool center=true, const bool invert=false, const char *valstr=NULL) {
    UNUSED(invert);
    int8_t n = LCD_WIDTH;
    lcd_moveto(0, row);
    if (center && !valstr) {
      int8_t pad = (LCD_WIDTH - utf8_strlen_P(pstr)) / 2;
      while (--pad >= 0) { lcd_put_wchar(' '); n--; }
    }
    n -= lcd_put_u8str_max_rom(pstr, n);
    if (valstr) n -= lcd_put_u8str_max(valstr, n);
    for (; n > 0; --n) lcd_put_wchar(' ');
  }

  static void lcd_implementation_drawmenu_generic(const bool sel, const uint8_t row, const char* pstr, const char pre_char, const char post_char) {
    uint8_t n = LCD_WIDTH - 2;
    lcd_moveto(0, row);
    lcd_put_wchar(sel ? pre_char : ' ');
    n -= lcd_put_u8str_max_rom(pstr, n);
    while (n--) lcd_put_wchar(' ');
    lcd_put_wchar(post_char);
  }

  static void lcd_implementation_drawmenu_setting_edit_generic(const bool sel, const uint8_t row, const char* pstr, const char pre_char, const char* const data) {
    uint8_t n = LCD_WIDTH - 2 - utf8_strlen(data);
    lcd_moveto(0, row);
    lcd_put_wchar(sel ? pre_char : ' ');
    n -= lcd_put_u8str_max_rom(pstr, n);
    lcd_put_wchar(':');
    while (n--) lcd_put_wchar(' ');
    lcd_put_u8str(data);
  }
  static void lcd_implementation_drawmenu_setting_edit_generic_P(const bool sel, const uint8_t row, const char* pstr, const char pre_char, const char* const data) {
    uint8_t n = LCD_WIDTH - 2 - utf8_strlen_P(data);
    lcd_moveto(0, row);
    lcd_put_wchar(sel ? pre_char : ' ');
    n -= lcd_put_u8str_max_rom(pstr, n);
    lcd_put_wchar(':');
    while (n--) lcd_put_wchar(' ');
    lcd_put_u8str_rom(data);
  }

  #define DRAWMENU_SETTING_EDIT_GENERIC(_src) lcd_implementation_drawmenu_setting_edit_generic(sel, row, pstr, '>', _src)
  #define DRAW_BOOL_SETTING(sel, row, pstr, data) lcd_implementation_drawmenu_setting_edit_generic_P(sel, row, pstr, '>', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))

  void lcd_implementation_drawedit(const char* pstr, const char* const value=NULL) {
    lcd_moveto(1, 1);
    lcd_put_u8str_rom(pstr);
    if (value != NULL) {
      lcd_put_wchar(':');
      int len = utf8_strlen(value);
      const uint8_t valrow = (utf8_strlen_P(pstr) + 1 + len + 1) > (LCD_WIDTH - 2) ? 2 : 1;  // Value on the next row if it won't fit
      lcd_moveto((LCD_WIDTH - 1) - (len + 1), valrow);                                       // Right-justified, padded by spaces
      lcd_put_wchar(' ');                                                                  // overwrite char if value gets shorter
      lcd_put_u8str(value);
    }
  }

  #if HAS_SD_SUPPORT

    static void lcd_implementation_drawmenu_sd(const bool sel, const uint8_t row, const char* const pstr, const char* longFilename, const uint8_t concat, const char post_char) {
      UNUSED(pstr);
      lcd_moveto(0, row);
      lcd_put_wchar(sel ? '>' : ' ');

      uint8_t n = LCD_WIDTH - concat;
      const char *outstr = longFilename;
      #if ENABLED(SCROLL_LONG_FILENAMES)
        if (sel) {
          uint8_t name_hash = row;
          for (uint8_t l = FILENAME_LENGTH; l--;)
            name_hash = ((name_hash << 1) | (name_hash >> 7)) ^ longFilename[l];  // rotate, xor
          if (filename_scroll_hash != name_hash) {                                // If the hash changed...
            filename_scroll_hash = name_hash;                                     // Save the new hash
            filename_scroll_max = max(0, utf8_strlen(longFilename) - n);           // Update the scroll limit
            filename_scroll_pos = 0;                                              // Reset scroll to the start
            lcd_status_update_delay = 8;                                          // Don't scroll right away
          }
          outstr += filename_scroll_pos;
        }
      #endif

      lcd_moveto(0, row);
      lcd_put_wchar(sel ? '>' : ' ');
      n -= lcd_put_u8str_max(outstr, n);

      while (n) { --n; lcd_put_wchar(' '); }
      lcd_put_wchar(post_char);
    }

    static void lcd_implementation_drawmenu_sdfile(const bool sel, const uint8_t row, const char* pstr, const char* longFilename) {
      lcd_implementation_drawmenu_sd(sel, row, pstr, longFilename, 2, ' ');
    }

    static void lcd_implementation_drawmenu_sddirectory(const bool sel, const uint8_t row, const char* pstr, const char* longFilename) {
      lcd_implementation_drawmenu_sd(sel, row, pstr, longFilename, 2, LCD_STR_FOLDER[0]);
    }

  #endif // SDSUPPORT

  #define lcd_implementation_drawmenu_back(sel, row, pstr, dummy) lcd_implementation_drawmenu_generic(sel, row, pstr, LCD_UPLEVEL_CHAR, LCD_UPLEVEL_CHAR)
  #define lcd_implementation_drawmenu_submenu(sel, row, pstr, data) lcd_implementation_drawmenu_generic(sel, row, pstr, '>', LCD_STR_ARROW_RIGHT[0])
  #define lcd_implementation_drawmenu_gcode(sel, row, pstr, gcode) lcd_implementation_drawmenu_generic(sel, row, pstr, '>', ' ')
  #define lcd_implementation_drawmenu_function(sel, row, pstr, data) lcd_implementation_drawmenu_generic(sel, row, pstr, '>', ' ')

  #if ENABLED(LCD_HAS_SLOW_BUTTONS)

    extern millis_t next_button_update_ms;

    static uint8_t lcd_implementation_read_slow_buttons() {
      #if ENABLED(LCD_I2C_TYPE_MCP23017)
        // Reading these buttons this is likely to be too slow to call inside interrupt context
        // so they are called during normal lcd_update
        uint8_t slow_bits = lcd.readButtons() << B_I2C_BTN_OFFSET;
        #if ENABLED(LCD_I2C_VIKI)
          if ((slow_bits & (B_MI | B_RI)) && PENDING(millis(), next_button_update_ms)) // LCD clicked
            slow_bits &= ~(B_MI | B_RI); // Disable LCD clicked buttons if screen is updated
        #endif // LCD_I2C_VIKI
        return slow_bits;
      #endif // LCD_I2C_TYPE_MCP23017
    }

  #endif // LCD_HAS_SLOW_BUTTONS

  #if ENABLED(LCD_HAS_STATUS_INDICATORS)

    static void lcd_implementation_update_indicators() {
      // Set the LEDS - referred to as backlights by the LiquidTWI2 library
      static uint8_t ledsprev = 0;
      uint8_t leds = 0;

      if (heaters[BED_INDEX].target_temperature > 0) leds |= LED_A;

      if (heaters[0].target_temperature > 0) leds |= LED_B;

      #if FAN_COUNT > 0
        if (0
          #if HAS_FAN0
            || fans[0].Speed
          #endif
          #if HAS_FAN1
            || fans[1].Speed
          #endif
          #if HAS_FAN2
            || fans[2].Speed
          #endif
          #if HAS_FAN3
            || fans[3].Speed
          #endif
        ) leds |= LED_C;
      #endif // FAN_COUNT > 0

      #if HOTENDS > 1
        if (thermalManager.degTargetHotend(1) > 0) leds |= LED_C;
      #endif

      if (leds != ledsprev) {
        lcd.setBacklight(leds);
        ledsprev = leds;
      }
    }

  #endif // LCD_HAS_STATUS_INDICATORS

  #if ENABLED(AUTO_BED_LEVELING_UBL)

    /**
      Possible map screens:

      16x2   |X000.00  Y000.00|
             |(00,00)  Z00.000|

      20x2   | X:000.00  Y:000.00 |
             | (00,00)   Z:00.000 |

      16x4   |+-------+(00,00)|
             ||       |X000.00|
             ||       |Y000.00|
             |+-------+Z00.000|

      20x4   | +-------+  (00,00) |
             | |       |  X:000.00|
             | |       |  Y:000.00|
             | +-------+  Z:00.000|
    */

    typedef struct {
      uint8_t custom_char_bits[ULTRA_Y_PIXELS_PER_CHAR];
    } custom_char;

    typedef struct {
      uint8_t column, row;
      uint8_t y_pixel_offset, x_pixel_offset;
      uint8_t x_pixel_mask;
    } coordinate;

    void add_edges_to_custom_char(custom_char * const custom, coordinate * const ul, coordinate * const lr, coordinate * const brc, const uint8_t cell_location);
    FORCE_INLINE static void clear_custom_char(custom_char * const cc) { ZERO(cc->custom_char_bits); }

    /*
    // This debug routine should be deleted by anybody that sees it.  It doesn't belong here
    // But I'm leaving it for now until we know the 20x4 Radar Map is working right.
    // We may need it again if any funny lines show up on the mesh points.
    void dump_custom_char(char *title, custom_char *c) {
      SERIAL_PROTOCOLLN(title);
      for (uint8_t j = 0; j < 8; j++) {
        for (uint8_t i = 7; i >= 0; i--)
          SERIAL_PROTOCOLCHAR(TEST(c->custom_char_bits[j], i) ? '1' : '0');
        SERIAL_EOL();
      }
      SERIAL_EOL();
    }
    //*/

    coordinate pixel_location(int16_t x, int16_t y) {
      coordinate ret_val;
      int16_t xp, yp, r, c;

      x++; y++; // +1 because lines on the left and top

      c = x / (ULTRA_X_PIXELS_PER_CHAR);
      r = y / (ULTRA_Y_PIXELS_PER_CHAR);

      ret_val.column = c;
      ret_val.row    = r;

      xp = x - c * (ULTRA_X_PIXELS_PER_CHAR);   // get the pixel offsets into the character cell
      xp = ULTRA_X_PIXELS_PER_CHAR - 1 - xp;    // column within relevant character cell (0 on the right)
      yp = y - r * (ULTRA_Y_PIXELS_PER_CHAR);

      ret_val.x_pixel_mask   = _BV(xp);
      ret_val.x_pixel_offset = xp;
      ret_val.y_pixel_offset = yp;
      return ret_val;
    }

    inline coordinate pixel_location(const uint8_t x, const uint8_t y) { return pixel_location((int16_t)x, (int16_t)y); }

    void lcd_implementation_ubl_plot(const uint8_t x, const uint8_t inverted_y) {

      #if LCD_WIDTH >= 20
        #define _LCD_W_POS 12
        #define _PLOT_X 1
        #define _MAP_X 3
        #define _LABEL(C,X,Y) lcd_moveto(X, Y); lcd_put_u8str(C)
        #define _XLABEL(X,Y) _LABEL("X:",X,Y)
        #define _YLABEL(X,Y) _LABEL("Y:",X,Y)
        #define _ZLABEL(X,Y) _LABEL("Z:",X,Y)
      #else
        #define _LCD_W_POS 8
        #define _PLOT_X 0
        #define _MAP_X 1
        #define _LABEL(X,Y,C) lcd_moveto(X, Y); lcd_put_wchar(C)
        #define _XLABEL(X,Y) _LABEL('X',X,Y)
        #define _YLABEL(X,Y) _LABEL('Y',X,Y)
        #define _ZLABEL(X,Y) _LABEL('Z',X,Y)
      #endif

      #if LCD_HEIGHT <= 3   // 16x2 or 20x2 display

        /**
         * Show X and Y positions
         */
        _XLABEL(_PLOT_X, 0);
        lcd_put_u8str(ftostr32(LOGICAL_X_POSITION(pgm_read_float(&ubl._mesh_index_to_xpos[x]))));

        _YLABEL(_LCD_W_POS, 0);
        lcd_put_u8str(ftostr32(LOGICAL_Y_POSITION(pgm_read_float(&ubl._mesh_index_to_ypos[inverted_y]))));

        lcd_moveto(_PLOT_X, 0);

      #else // 16x4 or 20x4 display

        coordinate upper_left, lower_right, bottom_right_corner;
        custom_char new_char;
        uint8_t i, j, k, l, m, n, n_rows, n_cols, y,
                bottom_line, right_edge,
                x_map_pixels, y_map_pixels,
                pixels_per_x_mesh_pnt, pixels_per_y_mesh_pnt,
                suppress_x_offset = 0, suppress_y_offset = 0;

        y = GRID_MAX_POINTS_Y - inverted_y - 1;

        upper_left.column  = 0;
        upper_left.row     = 0;
        lower_right.column = 0;
        lower_right.row    = 0;

        lcd_implementation_clear();

        x_map_pixels = (ULTRA_X_PIXELS_PER_CHAR) * (ULTRA_COLUMNS_FOR_MESH_MAP) - 2;  // minus 2 because we are drawing a box around the map
        y_map_pixels = (ULTRA_Y_PIXELS_PER_CHAR) * (ULTRA_ROWS_FOR_MESH_MAP) - 2;

        pixels_per_x_mesh_pnt = x_map_pixels / (GRID_MAX_POINTS_X);
        pixels_per_y_mesh_pnt = y_map_pixels / (GRID_MAX_POINTS_Y);

        if (pixels_per_x_mesh_pnt >= ULTRA_X_PIXELS_PER_CHAR) {         // There are only 2 custom characters available, so the X
          pixels_per_x_mesh_pnt = ULTRA_X_PIXELS_PER_CHAR;              // size of the mesh point needs to fit within them independent
          suppress_x_offset = 1;                                        // of where the starting pixel is located.
        }

        if (pixels_per_y_mesh_pnt >= ULTRA_Y_PIXELS_PER_CHAR) {         // There are only 2 custom characters available, so the Y
          pixels_per_y_mesh_pnt = ULTRA_Y_PIXELS_PER_CHAR;              // size of the mesh point needs to fit within them independent
          suppress_y_offset = 1;                                        // of where the starting pixel is located.
        }

        x_map_pixels = pixels_per_x_mesh_pnt * (GRID_MAX_POINTS_X);     // now we have the right number of pixels to make both
        y_map_pixels = pixels_per_y_mesh_pnt * (GRID_MAX_POINTS_Y);     // directions fit nicely

        right_edge = pixels_per_x_mesh_pnt * (GRID_MAX_POINTS_X) + 1;   // find location of right edge within the character cell
        bottom_line= pixels_per_y_mesh_pnt * (GRID_MAX_POINTS_Y) + 1;   // find location of bottome line within the character cell

        n_rows = bottom_line / (ULTRA_Y_PIXELS_PER_CHAR) + 1;
        n_cols = right_edge / (ULTRA_X_PIXELS_PER_CHAR) + 1;

        for (i = 0; i < n_cols; i++) {
          lcd_moveto(i, 0);
          lcd_put_wchar((char)0x00);                     // top line of the box

          lcd_moveto(i, n_rows - 1);
          lcd_put_wchar(0x01);                           // bottom line of the box
        }

        for (j = 0; j < n_rows; j++) {
          lcd_moveto(0, j);
          lcd_put_wchar(0x02);                           // Left edge of the box
          lcd_moveto(n_cols - 1, j);
          lcd_put_wchar(0x03);                           // right edge of the box
        }

        /**
         * If the entire 4th row is not in use, do not put vertical bars all the way down to the bottom of the display
         */

        k = pixels_per_y_mesh_pnt * (GRID_MAX_POINTS_Y) + 2;
        l = (ULTRA_Y_PIXELS_PER_CHAR) * n_rows;
        if (l > k && l - k >= (ULTRA_Y_PIXELS_PER_CHAR) / 2) {
          lcd_moveto(0, n_rows - 1);            // left edge of the box
          lcd_put_wchar(' ');
          lcd_moveto(n_cols - 1, n_rows - 1);   // right edge of the box
          lcd_put_wchar(' ');
        }

        clear_custom_char(&new_char);
        new_char.custom_char_bits[0] = 0b11111U;              // char #0 is used for the top line of the box
        lcd.createChar(0, (uint8_t*)&new_char);

        clear_custom_char(&new_char);
        k = (GRID_MAX_POINTS_Y) * pixels_per_y_mesh_pnt + 1;  // row of pixels for the bottom box line
        l = k % (ULTRA_Y_PIXELS_PER_CHAR);                    // row within relevant character cell
        new_char.custom_char_bits[l] = 0b11111U;              // char #1 is used for the bottom line of the box
        lcd.createChar(1, (uint8_t*)&new_char);

        clear_custom_char(&new_char);
        for (j = 0; j < ULTRA_Y_PIXELS_PER_CHAR; j++)
          new_char.custom_char_bits[j] = 0b10000U;            // char #2 is used for the left edge of the box
        lcd.createChar(2, (uint8_t*)&new_char);

        clear_custom_char(&new_char);
        m = (GRID_MAX_POINTS_X) * pixels_per_x_mesh_pnt + 1;  // Column of pixels for the right box line
        n = m % (ULTRA_X_PIXELS_PER_CHAR);                    // Column within relevant character cell
        i = ULTRA_X_PIXELS_PER_CHAR - 1 - n;                  // Column within relevant character cell (0 on the right)
        for (j = 0; j < ULTRA_Y_PIXELS_PER_CHAR; j++)
          new_char.custom_char_bits[j] = (uint8_t)_BV(i);     // Char #3 is used for the right edge of the box
        lcd.createChar(3, (uint8_t*)&new_char);

        i = x * pixels_per_x_mesh_pnt - suppress_x_offset;
        j = y * pixels_per_y_mesh_pnt - suppress_y_offset;
        upper_left = pixel_location(i, j);

        k = (x + 1) * pixels_per_x_mesh_pnt - 1 - suppress_x_offset;
        l = (y + 1) * pixels_per_y_mesh_pnt - 1 - suppress_y_offset;
        lower_right = pixel_location(k, l);

        bottom_right_corner = pixel_location(x_map_pixels, y_map_pixels);

        /**
         * First, handle the simple case where everything is within a single character cell.
         * If part of the Mesh Plot is outside of this character cell, we will follow up
         * and deal with that next.
         */

        //dump_custom_char("at entry:", &new_char);

        clear_custom_char(&new_char);
        const uint8_t ypix = min(upper_left.y_pixel_offset + pixels_per_y_mesh_pnt, ULTRA_Y_PIXELS_PER_CHAR);
        for (j = upper_left.y_pixel_offset; j < ypix; j++) {
          i = upper_left.x_pixel_mask;
          for (k = 0; k < pixels_per_x_mesh_pnt; k++) {
            new_char.custom_char_bits[j] |= i;
            i >>= 1;
          }
        }
        //dump_custom_char("after loops:", &new_char);

        add_edges_to_custom_char(&new_char, &upper_left, &lower_right, &bottom_right_corner, TOP_LEFT);
        //dump_custom_char("after add edges", &new_char);
        lcd.createChar(4, (uint8_t*)&new_char);

        lcd_moveto(upper_left.column, upper_left.row);
        lcd_put_wchar(0x04);
        //dump_custom_char("after lcd update:", &new_char);

        /**
         * Next, check for two side by side character cells being used to display the Mesh Point
         * If found...  do the right hand character cell next.
         */
        if (upper_left.column == lower_right.column - 1) {
          l = upper_left.x_pixel_offset;
          clear_custom_char(&new_char);
          for (j = upper_left.y_pixel_offset; j < ypix; j++) {
            i = _BV(ULTRA_X_PIXELS_PER_CHAR - 1);                  // Fill in the left side of the right character cell
            for (k = 0; k < pixels_per_x_mesh_pnt - 1 - l; k++) {
              new_char.custom_char_bits[j] |= i;
              i >>= 1;
            }
          }
          add_edges_to_custom_char(&new_char, &upper_left, &lower_right, &bottom_right_corner, TOP_RIGHT);

          lcd.createChar(5, (uint8_t *) &new_char);

          lcd_moveto(lower_right.column, upper_left.row);
          lcd_put_wchar(0x05);
        }

        /**
         * Next, check for two character cells stacked on top of each other being used to display the Mesh Point
         */
        if (upper_left.row == lower_right.row - 1) {
          l = ULTRA_Y_PIXELS_PER_CHAR - upper_left.y_pixel_offset;  // Number of pixel rows in top character cell
          k = pixels_per_y_mesh_pnt - l;                            // Number of pixel rows in bottom character cell
          clear_custom_char(&new_char);
          for (j = 0; j < k; j++) {
            i = upper_left.x_pixel_mask;
            for (m = 0; m < pixels_per_x_mesh_pnt; m++) {           // Fill in the top side of the bottom character cell
              new_char.custom_char_bits[j] |= i;
              if (!(i >>= 1)) break;
            }
          }
          add_edges_to_custom_char(&new_char, &upper_left, &lower_right, &bottom_right_corner, LOWER_LEFT);
          lcd.createChar(6, (uint8_t *) &new_char);

          lcd_moveto(upper_left.column, lower_right.row);
          lcd_put_wchar(0x06);
        }

        /**
         * Next, check for four character cells being used to display the Mesh Point.  If that is
         * what is here, we work to fill in the character cell that is down one and to the right one
         * from the upper_left character cell.
         */

        if (upper_left.column == lower_right.column - 1 && upper_left.row == lower_right.row - 1) {
          l = ULTRA_Y_PIXELS_PER_CHAR - upper_left.y_pixel_offset;   // Number of pixel rows in top character cell
          k = pixels_per_y_mesh_pnt - l;                             // Number of pixel rows in bottom character cell
          clear_custom_char(&new_char);
          for (j = 0; j < k; j++) {
            l = upper_left.x_pixel_offset;
            i = _BV(ULTRA_X_PIXELS_PER_CHAR - 1);                    // Fill in the left side of the right character cell
            for (m = 0; m < pixels_per_x_mesh_pnt - 1 - l; m++) {    // Fill in the top side of the bottom character cell
              new_char.custom_char_bits[j] |= i;
              i >>= 1;
            }
          }
          add_edges_to_custom_char(&new_char, &upper_left, &lower_right, &bottom_right_corner, LOWER_RIGHT);
          lcd.createChar(7, (uint8_t*)&new_char);

          lcd_moveto(lower_right.column, lower_right.row);
          lcd_put_wchar(0x07);
        }

      #endif

      /**
       * Print plot position
       */
      lcd_moveto(_LCD_W_POS, 0);
      lcd_put_wchar('(');
      lcd_put_u8str(itostr3(x));
      lcd_put_wchar(',');
      lcd_put_u8str(itostr3(inverted_y));
      lcd_put_wchar(')');

      #if LCD_HEIGHT <= 3   // 16x2 or 20x2 display

        /**
         * Print Z values
         */
        _ZLABEL(_LCD_W_POS, 1);
        if (!isnan(ubl.z_values[x][inverted_y]))
          lcd_put_u8str(ftostr43sign(ubl.z_values[x][inverted_y]));
        else
          lcd_put_u8str_rom(PSTR(" -----"));

      #else                 // 16x4 or 20x4 display

        /**
         * Show all values at right of screen
         */
        _XLABEL(_LCD_W_POS, 1);
        lcd_put_u8str(ftostr32(LOGICAL_X_POSITION(pgm_read_float(&ubl._mesh_index_to_xpos[x]))));
        _YLABEL(_LCD_W_POS, 2);
        lcd_put_u8str(ftostr32(LOGICAL_Y_POSITION(pgm_read_float(&ubl._mesh_index_to_ypos[inverted_y]))));

        /**
         * Show the location value
         */
        _ZLABEL(_LCD_W_POS, 3);
        if (!isnan(ubl.z_values[x][inverted_y]))
          lcd_put_u8str(ftostr43sign(ubl.z_values[x][inverted_y]));
        else
          lcd_put_u8str_rom(PSTR(" -----"));

      #endif // LCD_HEIGHT > 3
    }

    void add_edges_to_custom_char(custom_char * const custom, coordinate * const ul, coordinate * const lr, coordinate * const brc, uint8_t cell_location) {
      uint8_t i, k;
      int16_t n_rows = lr->row    - ul->row    + 1,
              n_cols = lr->column - ul->column + 1;

      /**
       * Check if Top line of box needs to be filled in
       */
      if (ul->row == 0 && ((cell_location & TOP_LEFT) || (cell_location & TOP_RIGHT))) {   // Only fill in the top line for the top character cells

        if (n_cols == 1) {
          if (ul->column != brc->column)
            custom->custom_char_bits[0] = 0xFF;                             // Single column in middle
          else
            for (i = brc->x_pixel_offset; i < ULTRA_X_PIXELS_PER_CHAR; i++) // Single column on right side
              SBI(custom->custom_char_bits[0], i);
        }
        else if ((cell_location & TOP_LEFT) || lr->column != brc->column)   // Multiple column in the middle or with right cell in middle
          custom->custom_char_bits[0] = 0xFF;
        else
          for (i = brc->x_pixel_offset; i < ULTRA_X_PIXELS_PER_CHAR; i++)
            SBI(custom->custom_char_bits[0], i);
      }

      /**
       * Check if left line of box needs to be filled in
       */
      if ((cell_location & TOP_LEFT) || (cell_location & LOWER_LEFT)) {
        if (ul->column == 0) {                                              // Left column of characters on LCD Display
          k = ul->row == brc->row ? brc->y_pixel_offset : ULTRA_Y_PIXELS_PER_CHAR; // If it isn't the last row... do the full character cell
          for (i = 0; i < k; i++)
            SBI(custom->custom_char_bits[i], ULTRA_X_PIXELS_PER_CHAR - 1);
        }
      }

      /**
       * Check if bottom line of box needs to be filled in
       */

      // Single row of mesh plot cells
      if (n_rows == 1 /* && (cell_location == TOP_LEFT || cell_location == TOP_RIGHT) */ && ul->row == brc->row) {
        if (n_cols == 1)                                                    // Single row, single column case
          k = ul->column == brc->column ? brc->x_pixel_mask : 0x01;
        else if (cell_location & TOP_RIGHT)                                 // Single row, multiple column case
          k = lr->column == brc->column ? brc->x_pixel_mask : 0x01;
        else                                                                // Single row, left of multiple columns
          k = 0x01;
        while (k < _BV(ULTRA_X_PIXELS_PER_CHAR)) {
          custom->custom_char_bits[brc->y_pixel_offset] |= k;
          k <<= 1;
        }
      }

      // Double row of characters on LCD Display
      // And this is a bottom custom character
      if (n_rows == 2 && (cell_location == LOWER_LEFT || cell_location == LOWER_RIGHT) && lr->row == brc->row) {
        if (n_cols == 1)                                                  // Double row, single column case
          k = ul->column == brc->column ? brc->x_pixel_mask : 0x01;
        else if (cell_location & LOWER_RIGHT)                             // Double row, multiple column case
          k = lr->column == brc->column ? brc->x_pixel_mask : 0x01;
        else                                                              // Double row, left of multiple columns
          k = 0x01;
        while (k < _BV(ULTRA_X_PIXELS_PER_CHAR)) {
          custom->custom_char_bits[brc->y_pixel_offset] |= k;
          k <<= 1;
        }
      }

      /**
       * Check if right line of box needs to be filled in
       */
      // Nothing to do if the lower right part of the mesh pnt isn't in the same column as the box line
      if (lr->column == brc->column) {
        // This mesh point is in the same character cell as the right box line
        if (ul->column == brc->column || (cell_location & TOP_RIGHT) || (cell_location & LOWER_RIGHT)) {
          // If not the last row... do the full character cell
          k = ul->row == brc->row ? brc->y_pixel_offset : ULTRA_Y_PIXELS_PER_CHAR;
          for (i = 0; i < k; i++) custom->custom_char_bits[i] |= brc->x_pixel_mask;
        }
      }
    }

  #endif // AUTO_BED_LEVELING_UBL

#endif // ULTIPANEL

#endif // ULTRALCD_IMPL_HD44780_H
