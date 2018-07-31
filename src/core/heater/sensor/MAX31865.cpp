/*
 * MAX31865.cpp
 *
 *      Author: Azarov
 *
 *      Based on Adafruit PT100/P1000 RTD Sensor w/MAX31865 library and MAX31865 sensor implementation in RepRapFirmware by dc42
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

#include "../../../../MK4duo.h"

#if ENABLED(SUPPORT_MAX31865)

namespace {

void readRegisterN(uint8_t addr, uint8_t buffer[], uint8_t n, const pin_t cs_pin) {
  addr &= 0x7F; // make sure top bit is not set

  HAL::spiBegin();
  HAL::digitalWrite(cs_pin, LOW); // enable TT_MAX31855

  // ensure 100ns delay - a bit extra is fine
  #if ENABLED(ARDUINO_ARCH_SAM)
    HAL::delayMicroseconds(1);
  #else
    asm("nop"); // 50ns on 20Mhz, 62.5ns on 16Mhz
    asm("nop"); // 50ns on 20Mhz, 62.5ns on 16Mhz
  #endif

  HAL::spiSend(addr);

  //Serial.print("$"); Serial.print(addr, HEX); Serial.print(": ");
  while (n--) {
    buffer[0] = HAL::spiReceive();
    //Serial.print(" 0x"); Serial.print(buffer[0], HEX);
    buffer++;
  }
  //Serial.println();


  HAL::digitalWrite(cs_pin, HIGH); // disable TT_MAX31855
}

uint8_t readRegister8(uint8_t addr, const pin_t cs_pin) {
  uint8_t ret = 0;
  readRegisterN(addr, &ret, 1, cs_pin);

  return ret;
}

uint16_t readRegister16(uint8_t addr, const pin_t cs_pin) {
  uint8_t buffer[2] = {0, 0};
  readRegisterN(addr, buffer, 2, cs_pin);

  uint16_t ret = buffer[0];
  ret <<= 8;
  ret |=  buffer[1];

  return ret;
}


void writeRegister8(uint8_t addr, uint8_t data, const pin_t cs_pin) {

  HAL::spiBegin();
  HAL::digitalWrite(cs_pin, LOW); // enable TT_MAX31855

  // ensure 100ns delay - a bit extra is fine
  #if ENABLED(ARDUINO_ARCH_SAM)
    HAL::delayMicroseconds(1);
  #else
    asm("nop"); // 50ns on 20Mhz, 62.5ns on 16Mhz
    asm("nop"); // 50ns on 20Mhz, 62.5ns on 16Mhz
  #endif

  HAL::spiSend(addr | 0x80); // make sure top bit is set
  HAL::spiSend(data);

  //Serial.print("$"); Serial.print(addr, HEX); Serial.print(" = 0x"); Serial.println(data, HEX);

  HAL::digitalWrite(cs_pin, HIGH); // disable TT_MAX31855
}

void clearFault(const pin_t cs_pin) {
  uint8_t t = readRegister8(MAX31856_CONFIG_REG, cs_pin);
  t &= ~0x2C;
  t |= MAX31856_CONFIG_FAULTSTAT;
  writeRegister8(MAX31856_CONFIG_REG, t, cs_pin);
}

uint8_t readFault(const pin_t cs_pin) {
  return readRegister8(MAX31856_FAULTSTAT_REG, cs_pin);
}

bool readRTD (uint16_t &rtd, const pin_t cs_pin) {

  //uint8_t r0 = readRegister8(MAX31856_CONFIG_REG, cs_pin);
  rtd = readRegister16(MAX31856_RTDMSB_REG, cs_pin);

  if ((rtd & 1) != 0)										// if fault bit set
  {
	  uint8_t f = readFault(cs_pin);
	  if (f & 0x04) SERIAL_EM("MAX31856 error - over/undervoltage");
	  else if (f & 0x13) SERIAL_EM("MAX31856 error - open circuit");
	  else SERIAL_EM("MAX31856 hardware error");
	  clearFault(cs_pin);
	  return false;
  }

  // remove fault
  rtd >>= 1;
  return true;
}

}


bool MAX31865::Initialize(max31865_numwires_t wires, const pin_t cs_pin) {
	HAL::pinMode(cs_pin, OUTPUT_HIGH);
	//start hardware SPI

	HAL::spiBegin();

	uint8_t r0 = DefaultCr0;
	  if (wires == MAX31865_3WIRE) {
		r0 |= MAX31856_CONFIG_3WIRE;
	  } else {
	    // 2 or 4 wire
		r0 &= ~MAX31856_CONFIG_3WIRE;
	  }
	  writeRegister8(MAX31856_CONFIG_REG, r0, cs_pin);

	//Serial.print("config: "); Serial.println(readRegister8(MAX31856_CONFIG_REG), HEX);
	return true;
}

float MAX31865::ReadTemperature(const pin_t cs_pin) {
	  // http://www.analog.com/media/en/technical-documentation/application-notes/AN709_0.pdf

	  float RTDnominal = RTD_RNOMINAL;
	  float refResistor = RTD_RREF;

	  float Z1, Z2, Z3, Z4, Rt, temp;

	  uint16_t rtd;
	  if (!readRTD(rtd, cs_pin)) return 1000;

	  Rt = rtd;
	  Rt /= 32768;
	  Rt *= refResistor;

	  //Serial.print("Resistance: "); Serial.println(Rt, 8);

	  Z1 = -RTD_A;
	  Z2 = RTD_A * RTD_A - (4 * RTD_B);
	  Z3 = (4 * RTD_B) / RTDnominal;
	  Z4 = 2 * RTD_B;

	  temp = Z2 + (Z3 * Rt);
	  temp = (sqrt(temp) + Z1) / Z4;

	  if (temp >= 0) return temp;

	  // ugh.
	  float rpoly = Rt;

	  temp = -242.02;
	  temp += 2.2228 * rpoly;
	  rpoly *= Rt;  // square
	  temp += 2.5859e-3 * rpoly;
	  rpoly *= Rt;  // ^3
	  temp -= 4.8260e-6 * rpoly;
	  rpoly *= Rt;  // ^4
	  temp -= 2.8183e-8 * rpoly;
	  rpoly *= Rt;  // ^5
	  temp += 1.5243e-10 * rpoly;

	  return temp;
}

#endif
