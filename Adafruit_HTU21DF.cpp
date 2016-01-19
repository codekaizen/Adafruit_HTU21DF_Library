/*************************************************** 
  This is a library for the HTU21DF Humidity & Temp Sensor

  Designed specifically to work with the HTU21DF sensor from Adafruit
  ----> https://www.adafruit.com/products/1899

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_HTU21DF.h"
#if defined(__AVR__)
#include <util/delay.h>
#endif

Adafruit_HTU21DF::Adafruit_HTU21DF() {
}


boolean Adafruit_HTU21DF::begin(void) {
  Wire.begin();
  
  reset();

  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(HTU21DF_READREG);
  Wire.endTransmission();
  Wire.requestFrom(HTU21DF_I2CADDR, 1);
  return (Wire.read() == 0x2); // after reset should be 0x2
}

void Adafruit_HTU21DF::reset(void) {
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(HTU21DF_RESET);
  Wire.endTransmission();
  delay(15);
}


float Adafruit_HTU21DF::readTemperature(void) {
  
  // OK lets ready!
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(HTU21DF_READTEMP);
  Wire.endTransmission();
  
  delay(50); // add delay between request and actual read!
  
  Wire.requestFrom(HTU21DF_I2CADDR, 3);
  while (!Wire.available()) {}

  uint16_t t = Wire.read();
  t <<= 8;
  t |= Wire.read();

  uint8_t crc = Wire.read();
  uint8_t computedCrc = computeCrc((uint8_t*)&t, 0, 2);

  float temp = t;
  temp *= 175.72;
  temp /= 65536;
  temp -= 46.85;

  return temp;
}
  

float Adafruit_HTU21DF::readHumidity(void) {
  // OK lets ready!
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(HTU21DF_READHUM);
  Wire.endTransmission();
  
  delay(50); // add delay between request and actual read!
  
  Wire.requestFrom(HTU21DF_I2CADDR, 3);
  while (!Wire.available()) {}

  uint16_t h = Wire.read();
  h <<= 8;
  h |= Wire.read();

  uint8_t crc = Wire.read();
  uint8_t computedCrc = computeCrc((uint8_t*)&h, 0, 2);

  float hum = h;
  hum *= 125;
  hum /= 65536;
  hum -= 6;

  return hum;
}


uint8_t computeCrc(uint8_t* data, int start, int length)
{
	uint8_t crc[8];
	
	for (int c = 0; c < 8; c++)
	{ 
		crc[c] = 0; 
	}
	
	int bitOffset = 0;

	for (int i = 0; i < 8 * length; i++)
	{
		uint8_t current = data[start + i / 8];
		int bitPosition = 7 - bitOffset++ % 8;
		uint8_t mask = 1;
		uint8_t invert = ((current >> bitPosition) & mask) ^ crc[7];
		crc[7] = crc[6];
		crc[6] = crc[5];
		crc[5] = ((crc[4] ^ invert) & 0x1);
		crc[4] = ((crc[3] ^ invert) & 0x1);
		crc[3] = crc[2];
		crc[2] = crc[1];
		crc[1] = crc[0];
		crc[0] = invert;
	}

	uint8_t final = 0;
	
	for (int f = 7; f >= 0; f--)
	{
		final |= crc[f];
		final = (final << 1);
	}

	return final;
}


/*********************************************************************/
