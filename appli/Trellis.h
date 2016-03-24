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

#define I2C_PORT            GPIOB
#define I2C_SIOC_PIN        GPIO_PIN_9
#define I2C_SIOD_PIN        GPIO_PIN_6

#define I2C_SIOC_H()        HAL_GPIO_WritePin(I2C_PORT, I2C_SIOC_PIN, GPIO_PIN_SET)
#define I2C_SIOC_L()        HAL_GPIO_WritePin(I2C_PORT, I2C_SIOC_PIN, GPIO_PIN_RESET)

#define I2C_SIOD_H()        HAL_GPIO_WritePin(I2C_PORT, I2C_SIOD_PIN, GPIO_PIN_SET)
#define I2C_SIOD_L()        HAL_GPIO_WritePin(I2C_PORT, I2C_SIOD_PIN, GPIO_PIN_RESET)

#define I2C_SIOD_READ()     HAL_GPIO_ReadPin(I2C_PORT, I2C_SIOD_PIN)
#define I2C_SIOD_WRITE(bit) HAL_GPIO_WritePin(I2C_PORT, I2C_SIOD_PIN, bit);

#define ACK 0
#define NACK 1

uint16_t displaybuffer[8]; // led buffere

// utile

/* Prototypes */
void trellis_init();
void trellis_setLed(uint8_t x);
void trellis_clearLed(uint8_t x);
void trellis_display();

void i2c_start(void);
void i2c_stop(void);
char i2c_write_byte(uint8_t data);

uint8_t keys[6], lastkeys[6];
uint8_t i2c_addr;

#endif // _TRELLIS_H_
