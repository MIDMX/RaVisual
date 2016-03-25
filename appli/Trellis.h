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
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"


uint16_t displaybuffer[8]; // led buffer

/* Prototypes */
void trellis_init();
void trellis_setLed(uint8_t x);
void trellis_clearLed(uint8_t x);
void trellis_display();
void trellis_setBrightness(uint8_t val);

uint8_t keys[6], lastkeys[6];
uint8_t i2c_addr;

#endif // _TRELLIS_H_
