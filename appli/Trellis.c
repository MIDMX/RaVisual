#include "Trellis.h"
#include "stm32f4_sys.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_i2c.h"


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

	i2c_start();
	i2c_write_byte(0xE0);
	i2c_write_byte(HT16K33_CMD_BRIGHTNESS | 7);
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
	uint16_t val = ~(1<<(x & 0x0F));
	uint16_t page = x >> 4;
	displaybuffer[page] &= val;
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

/*Big thanks to kwagyeman  -> https://github.com/kwagyeman*/
void delay(void) {
    for(volatile int i=0; i<60; i++);
}

void i2c_start(void) {
    /* The start of data transmission occurs when
       SIO_D is driven low while SIO_C is high */
    I2C_SIOD_L();
    delay();
    I2C_SIOC_L();
    delay();
}

void i2c_stop(void) {
    /* The stop of data transmission occurs when
       SIO_D is driven high while SIO_C is high */
    I2C_SIOC_H();
    delay();
    I2C_SIOD_H();
    delay();
}

char i2c_write_byte(uint8_t data) {
    char i;

    for(i=0; i<8; i++) {
        I2C_SIOD_WRITE((data >> (7 - i)) & 1);
        delay();
        I2C_SIOC_H();
        delay();
        I2C_SIOC_L();
        delay();
    }

    I2C_SIOD_H();
    delay();

    I2C_SIOC_H();
    delay();

    /* Read ACK */
    i = I2C_SIOD_READ();
    delay();

    I2C_SIOC_L();
    delay();

    I2C_SIOD_L();
    delay();
    return i;
}



int soft_i2c_write_bytes(uint8_t slv_addr, uint8_t *buf, int len, bool_e stop)
{
    int ret = 0;
    i2c_start();
    ret |= i2c_write_byte(slv_addr);
    for(int i=0; i<len; i++) {
        ret |= i2c_write_byte(buf[i]);
    }
    if (stop) {
        i2c_stop();
    } else {
        I2C_SIOD_H();
        delay();
        I2C_SIOC_H();
        delay();
    }
    return ret;
}

void soft_i2c_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_OD;

    GPIO_InitStructure.Pin = I2C_SIOC_PIN;
    I2C_SIOC_H(); // Set first to prevent glitches.
    HAL_GPIO_Init(I2C_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = I2C_SIOD_PIN;
    I2C_SIOD_H(); // Set first to prevent glitches.
    HAL_GPIO_Init(I2C_PORT, &GPIO_InitStructure);

    for(volatile int i=0; i<1000; i++);

    for(int j=0; j<127; j++) { // initialize bus
        soft_i2c_write_bytes(j << 1, NULL, 0, TRUE);
    }
}

