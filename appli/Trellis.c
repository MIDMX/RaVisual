#include "Trellis.h"
#include "stm32f4_sys.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"

#define HT16K33_BLINK_CMD       0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_CMD_BRIGHTNESS  0xE0

#define SLAVE_ADDRESS 0x4A // the slave address (example)
//ffuuuuuu


/*
void begin() {

  I2C1_init(); // initialize I2C peripheral
  I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0x01); // write one byte to the slave
  I2C_stop(I2C1); // stop the transmission
  blinkRate(HT16K33_BLINK_OFF);
  
  setBrightness(15); // max brightness

  I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0x01); // write one byte to the slave
  I2C_stop(I2C1); // stop the transmission

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
  I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0x01); // write one byte to the slave
  I2C_stop(I2C1); // stop the transmission
  I2C_write(I2C1, HT16K33_CMD_BRIGHTNESS | b);
  I2C_stop(I2C1); // stop the transmission
}

void blinkRate(uint8_t b) {
  I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);
  if (b > 3) b = 0; // turn off if not sure
  I2C_write(I2C1,HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b << 1));
  I2C_stop(I2C1);
}


void writeDisplay(void) {
I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);
I2C_write(I2C1,(uint8_t)0x00); // start at address $00

  for (uint8_t i=0; i<8; i++) {
	  I2C_write(I2C1,displaybuffer[i] & 0xFF);
	  I2C_write(I2C1,displaybuffer[i] >> 8);
  }
  I2C_stop(I2C1);
}

void clear(void) {
  memset(displaybuffer, 0, sizeof(displaybuffer));
}
*/
