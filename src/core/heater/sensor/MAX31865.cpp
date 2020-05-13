/*
 * MAX31865.cpp
 *
 *      Author: Avazar
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

  HAL::spiInit(MAX_31865_CHANNEL, SPI_MAX_31865_RATE);
  HAL::digitalWrite(cs_pin, LOW); // enable MAX31865

  // ensure 100ns delay
  HAL::delayNanoseconds(100);

  HAL::spiSend(MAX_31865_CHANNEL, addr);

  //Serial.print("$"); Serial.print(addr, HEX); Serial.print(": ");
  while (n--) {
	  buffer[0] = HAL::spiReceive(MAX_31865_CHANNEL);
    buffer++;
  }

  HAL::digitalWrite(cs_pin, HIGH); // disable MAX31865
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

  HAL::spiInit(MAX_31865_CHANNEL, SPI_MAX_31865_RATE);
  HAL::digitalWrite(cs_pin, LOW); // enable MAX31865

  // ensure 100ns delay
  HAL::delayNanoseconds(100);

  HAL::spiSend(MAX_31865_CHANNEL, addr | 0x80);
  HAL::spiSend(MAX_31865_CHANNEL, data);

  HAL::digitalWrite(cs_pin, HIGH); // disable TT_MAX31865
}

void clearFault(const pin_t cs_pin) {
  uint8_t t = readRegister8(MAX31865_CONFIG_REG, cs_pin);
  t &= ~0x2C;
  t |= MAX31865_CONFIG_FAULTSTAT;
  writeRegister8(MAX31865_CONFIG_REG, t, cs_pin);
}

uint8_t readFault(const pin_t cs_pin) {
  return readRegister8(MAX31865_FAULTSTAT_REG, cs_pin);
}

bool readRTD(uint16_t &rtd, const pin_t cs_pin) {

  rtd = readRegister16(MAX31865_RTDMSB_REG, cs_pin);

  if ((rtd & 1) != 0)										// if fault bit set
  {
	  uint8_t f = readFault(cs_pin);
	  //Serial.print("R0:");
	  //Serial.println(r0, HEX);
	  //Serial.print("RTD:");
	  //Serial.println(rtd, HEX);
	  //Serial.print("F:");
	  //Serial.println(f, HEX);
	  if (f & MAX31865_FAULT_OVUV) SERIAL_LMV(WARNING, PSTR("MAX31865 error - over/undervoltage - "), cs_pin);
	  else if (f & MAX31865_FAULT_OPEN_CIRCUIT) SERIAL_LMV(ER, PSTR("MAX31865 error - open circuit - "), cs_pin);
	  else if (f & MAX31865_FAULT_LOWTHRESH) SERIAL_LMV(ER,PSTR("MAX31865 error - RDT low resistance - "), cs_pin);
	  else SERIAL_LMV(ER,PSTR("MAX31865 hardware error - "), cs_pin);
	  clearFault(cs_pin);
  }

  // remove fault
  rtd >>= 1;
  return true;
}

}


bool MAX31865::Initialize(max31865_numwires_t wires, const pin_t cs_pin) {
	HAL::pinMode(cs_pin, OUTPUT_HIGH);

	//Serial.print("config to set: "); Serial.println(DefaultCr0, HEX);
	writeRegister8(MAX31865_CONFIG_REG, DefaultCr0, cs_pin);

	//Serial.print("config after: "); Serial.println(readRegister8(MAX31865_CONFIG_REG, cs_pin), HEX);
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
