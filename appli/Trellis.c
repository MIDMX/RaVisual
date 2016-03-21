

#include "Trellis.h"
#include "stm32f4xx_hal.h"
#include "macro_types.h"
#include "main.h"
#include "stm32f4_uart.h"
#include "stm32f4_sys.h"
#include "stm32f4xx_hal_i2c.h"

#define SLAVE_ADDRESS 0x3D // the slave address (example)
#define HT16K33_BLINK_CMD       0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_CMD_BRIGHTNESS  0xE0

I2C_Init(I2C1, TM_I2C_PinsPack_1, 100000);

/*
void begin(uint8_t _addr = 0x70) {
  i2c_addr = _addr;

  Wire.begin();

  Wire.beginTransmission(i2c_addr);
  Wire.write(0x21);  // turn on oscillator
  Wire.endTransmission();
  blinkRate(HT16K33_BLINK_OFF);
  
  setBrightness(15); // max brightness

  Wire.beginTransmission(i2c_addr);
  Wire.write(0xA1);  // turn on interrupt, active low
  Wire.endTransmission();

}


void setLED(uint8_t x) {
  if (x > 15) return;
  x = pgm_read_byte(&ledLUT[x]);
  displaybuffer[x >> 4] |= _BV(x & 0x0F);
}
void clrLED(uint8_t x) {
  if (x > 15) return;
  x = pgm_read_byte(&ledLUT[x]);
  displaybuffer[x >> 4] &= ~_BV(x & 0x0F);
}


void setBrightness(uint8_t b) {
  if (b > 15) b = 15;
  Wire.beginTransmission(i2c_addr);
  Wire.write(HT16K33_CMD_BRIGHTNESS | b);
  Wire.endTransmission();  
}

void blinkRate(uint8_t b) {
  Wire.beginTransmission(i2c_addr);
  if (b > 3) b = 0; // turn off if not sure
  
  Wire.write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b << 1)); 
  Wire.endTransmission();
}


void writeDisplay(void) {
  Wire.beginTransmission(i2c_addr);
  Wire.write((uint8_t)0x00); // start at address $00

  for (uint8_t i=0; i<8; i++) {
    Wire.write(displaybuffer[i] & 0xFF);    
    Wire.write(displaybuffer[i] >> 8);    
  }
  Wire.endTransmission();  
}

void clear(void) {
  memset(displaybuffer, 0, sizeof(displaybuffer));
}
*/
