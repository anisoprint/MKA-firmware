/*
 * MAX31865.h
 *
 *      Author: Azarov
 *
 *      Based on Adafruit PT100/P1000 RTD Sensor w/MAX31865 library
 */

/***************************************************
  This is a library for the Adafruit PT100/P1000 RTD Sensor w/MAX31865

  Designed specifically to work with the Adafruit RTD Sensor
  ----> https://www.adafruit.com/products/3328

  This sensor uses SPI to communicate, 4 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#ifndef SRC_CORE_HEATER_SENSOR_MAX31865_H_
#define SRC_CORE_HEATER_SENSOR_MAX31865_H_



#if ENABLED(SUPPORT_MAX31865)

#define MAX_31865_CHANNEL 3
#define SPI_MAX_31865_RATE 0

#define MAX31856_CONFIG_REG            0x00
#define MAX31856_CONFIG_BIAS           0x80
#define MAX31856_CONFIG_MODEAUTO       0x40
#define MAX31856_CONFIG_MODEOFF        0x00
#define MAX31856_CONFIG_1SHOT          0x20
#define MAX31856_CONFIG_3WIRE          0x10
#define MAX31856_CONFIG_24WIRE         0x00
#define MAX31856_CONFIG_FAULTSTAT      0x02
#define MAX31856_CONFIG_FILT50HZ       0x01
#define MAX31856_CONFIG_FILT60HZ       0x00

#define MAX31856_RTDMSB_REG           0x01
#define MAX31856_RTDLSB_REG           0x02
#define MAX31856_HFAULTMSB_REG        0x03
#define MAX31856_HFAULTLSB_REG        0x04
#define MAX31856_LFAULTMSB_REG        0x05
#define MAX31856_LFAULTLSB_REG        0x06
#define MAX31856_FAULTSTAT_REG        0x07


#define MAX31865_FAULT_HIGHTHRESH     0x80
#define MAX31865_FAULT_LOWTHRESH      0x40
#define MAX31865_FAULT_REFINLOW       0x20
#define MAX31865_FAULT_REFINHIGH      0x10
#define MAX31865_FAULT_RTDINLOW       0x08
#define MAX31865_FAULT_OVUV           0x04


#define RTD_A 3.9083e-3
#define RTD_B -5.775e-7

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RTD_RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RTD_RNOMINAL  100.0

typedef enum max31865_numwires {
  MAX31865_2WIRE = 0,
  MAX31865_3WIRE = 1,
  MAX31865_4WIRE = 0
} max31865_numwires_t;

// Default configuration register
// Note that to get the MAX31865 to do continuous conversions, we need to set the bias bit as well as the continuous-conversion bit
//  Vbias=1
//  Conversion mode=1
//	1shot = 0
//	3wire=0
//	Fault detection=00 no action
//	Fault status=1 clear any existing fault
//	50/60Hz reject=1 for 50Hz (0 for 60Hz)
const uint8_t DefaultCr0 = 0b11000011;
const uint8_t Cr0ReadMask = 0b11011101;

namespace MAX31865 {

	bool Initialize(max31865_numwires_t wires, const pin_t cs_pin);
	float ReadTemperature(const pin_t cs_pin);
}

#endif

#endif /* SRC_CORE_HEATER_SENSOR_MAX31865_H_ */
