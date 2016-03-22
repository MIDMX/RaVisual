/*************************************************** 
  This is a library for the Adafruit Trellis w/HT16K33
  Designed specifically to work with the Adafruit Trellis 
  ----> https://www.adafruit.com/products/1616
  ----> https://www.adafruit.com/products/1611
  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.  
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _TRELLIS_H_
#define _TRELLIS_H_

#define LED_ON  1
#define LED_OFF 0

#define HT16K33_BLINK_OFF    0
#define HT16K33_BLINK_2HZ    1
#define HT16K33_BLINK_1HZ    2
#define HT16K33_BLINK_HALFHZ 3

#include "macro_types.h"

// this is the raw HT16K33 controller
  void begin();
  void setBrightness(uint8_t b);
  void blinkRate(uint8_t b);
  void writeDisplay(void);
  void clear(void);
  void setLED(uint8_t x);
  void clrLED(uint8_t x);
  void init(uint8_t a);
  void I2C1_init(void);

  uint16_t displaybuffer[8];
  uint8_t keys[6], lastkeys[6];
  uint8_t i2c_addr;

#endif // _TRELLIS_H_
