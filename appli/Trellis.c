#include "Trellis.h"
#include "stm32f4_sys.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"
#include "easy2c.h"fhy;jtu;lr-j-''


#define HT16K33_BLINK_CMD       0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_CMD_BRIGHTNESS  0xE0
#define I2C_ADDRESS       	    0x70

I2C_HandleTypeDef I2CHandle;

/*Big thanks to Rascafr love u babe */
unsigned char ledLUT[16] =
{ 0x3A, 0x37, 0x35, 0x34,
  0x28, 0x29, 0x23, 0x24,
  0x16, 0x1B, 0x11, 0x10,
  0x0E, 0x0D, 0x0C, 0x02 };

/* Functions */
void trellis_init() {
	i2c_start();
	i2c_write_byte(0xE0);
	i2c_write_byte(0x21);
	i2c_stop();

	for (int i=0;i<200;i++);

	i2c_start();
	i2c_write_byte(0xE0);
	//i2c_write_byte(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON|(HT16K33_BLINK_1HZ<<1));
	i2c_write_byte(0x81);
	i2c_stop();

	for (int i=0;i<200;i++);

	trellis_setBrightness(15);
}

void trellis_setBrightness(uint8_t val){
	if (val > 15 ){
		val = 15;
	}
	i2c_start();
	i2c_write_byte(0xE0);
	i2c_write_byte(HT16K33_CMD_BRIGHTNESS | val);
	i2c_stop();
}

void trellis_setLed(uint8_t x) {
	if (x > 15) return;
	x = ledLUT[x];
	uint16_t val = (1<<(x & 0x0F));
	uint16_t page = x >> 4;
	displaybuffer[page] |= val;
}

void trellis_clearLed(uint8_t x) { // TODO BUG
	if (x > 15) return;
	x = ledLUT[x];
	uint16_t val = (1<<(x & 0x0F));
	uint16_t page = x >> 4;
	displaybuffer[page] &= ~val;
}

void trellis_display() {
	i2c_start();
	i2c_write_byte(0xE0);
	i2c_write_byte(0x00); // 1st addr

	for (int l=0;l<8;l++) {
		i2c_write_byte(displaybuffer[l] & 0xFF);
		i2c_write_byte(displaybuffer[l] >> 8);
	}

	i2c_stop();
}


