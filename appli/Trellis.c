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

/*Big thanks to kwagyeman  -> https://github.com/kwagyeman*/
static void delay(void) // TODO: Update with clock speed knowledge for M7.
{
    for(volatile int i=0; i<16; i++);
}

static void i2c_start(void)
{
    /* The start of data transmission occurs when
       SIO_D is driven low while SIO_C is high */
    I2C_SIOD_L();
    delay();
    I2C_SIOC_L();
    delay();
}

static void i2c_stop(void)
{
    /* The stop of data transmission occurs when
       SIO_D is driven high while SIO_C is high */
    I2C_SIOC_H();
    delay();
    I2C_SIOD_H();
    delay();
}



static char i2c_write_byte(uint8_t data)
{
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
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
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


void begin() {
	i2c_start();
	//soft_i2c_write_bytes(0x00,(uint8_t*)0x00,16,FALSE); //TODO: check with oscilloscope
	i2c_write_byte(0x08); //	<- this works
	i2c_stop();
}

/*
void setLED(uint8_t x) {
  if (x > 15) return;
 // x = pgm_read_byte(&ledLUT[x]);
  displaybuffer[x >> 4] |= _BV(x & 0x0F);
}
void clrLED(uint8_t x) {
  if (x > 15) return;
//  x = pgm_read_byte(&ledLUT[x]);
  displaybuffer[x >> 4] &= ~_BV(x & 0x0F);
}


void setBrightness(uint8_t b) {
  if (b > 15) b = 15;
 // I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0x01); // write one byte to the slave
  I2C_stop(I2C1); // stop the transmission
  I2C_write(I2C1, HT16K33_CMD_BRIGHTNESS | b);
  I2C_stop(I2C1); // stop the transmission
}

void blinkRate(uint8_t b) {
//  I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);
  if (b > 3) b = 0; // turn off if not sure
  I2C_write(I2C1,HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b << 1));
  I2C_stop(I2C1);
}


void writeDisplay(void) {
//I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);
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
