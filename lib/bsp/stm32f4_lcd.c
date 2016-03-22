/**
  ******************************************************************************
  * @file    stm324xg_discovery_lcd.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-September-2011
  * @brief   This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320)
  *          and AM240320D5TOQW01H (LCD_ILI9325) Liquid Crystal Display Modules
  *          of STM324xG-EVAL evaluation board(MB786) RevB.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; Portions COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 
/**
  ******************************************************************************
  * <h2><center>&copy; Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.</center></h2>
  * @file    stm32f4_discovery_lcd.c
  * @author  CMP Team
  * @version V1.0.0
  * @date    28-December-2012
  * @brief   LCD LOW_LEVEL Drive 
  *          Modified to support the STM32F4DISCOVERY, STM32F4DIS-BB, STM32F4DIS-CAM
  *          and STM32F4DIS-LCD modules.     
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, Embest SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
  * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
  * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
  * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4_lcd.h"
#include "stm32f4xx_hal.h"
#include "macro_types.h"
#include "lcd_display_form_and_text.h"
#include "stm32f4_gpio.h"

/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32F4_DISCOVERY
  * @{
  */
    
/** @defgroup stm32f4_discovery_LCD 
  * @brief This file includes the LCD driver for (LCDSSD2119)
  * @{
  */ 

/** @defgroup stm32f4_discovery_LCD_Private_TypesDef
  * @{
  */

/** @defgroup stm32f4_discovery_LCD_Private define
  * @{
  */
#define LCD_RST_PIN                  (GPIO_PIN_3)
#define LCD_RST_PORT                 (GPIOD)

#define LCD_PWM_PIN                  (GPIO_PIN_13)
#define LCD_PWM_PORT                 (GPIOD)


/**
  * @brief Various internal SD2119 registers name labels
  */
#define SSD2119_DEVICE_CODE_READ_REG  0x00
#define SSD2119_OSC_START_REG         0x00
#define SSD2119_OUTPUT_CTRL_REG       0x01
#define SSD2119_LCD_DRIVE_AC_CTRL_REG 0x02
#define SSD2119_PWR_CTRL_1_REG        0x03
#define SSD2119_DISPLAY_CTRL_REG      0x07
#define SSD2119_FRAME_CYCLE_CTRL_REG  0x0B
#define SSD2119_PWR_CTRL_2_REG        0x0C
#define SSD2119_PWR_CTRL_3_REG        0x0D
#define SSD2119_PWR_CTRL_4_REG        0x0E
#define SSD2119_GATE_SCAN_START_REG   0x0F
#define SSD2119_SLEEP_MODE_1_REG      0x10
#define SSD2119_ENTRY_MODE_REG        0x11
#define SSD2119_SLEEP_MODE_2_REG      0x12
#define SSD2119_GEN_IF_CTRL_REG       0x15
#define SSD2119_PWR_CTRL_5_REG        0x1E
#define SSD2119_RAM_DATA_REG          0x22
#define SSD2119_FRAME_FREQ_REG        0x25
#define SSD2119_ANALOG_SET_REG        0x26
#define SSD2119_VCOM_OTP_1_REG        0x28
#define SSD2119_VCOM_OTP_2_REG        0x29
#define SSD2119_GAMMA_CTRL_1_REG      0x30
#define SSD2119_GAMMA_CTRL_2_REG      0x31
#define SSD2119_GAMMA_CTRL_3_REG      0x32
#define SSD2119_GAMMA_CTRL_4_REG      0x33
#define SSD2119_GAMMA_CTRL_5_REG      0x34
#define SSD2119_GAMMA_CTRL_6_REG      0x35
#define SSD2119_GAMMA_CTRL_7_REG      0x36
#define SSD2119_GAMMA_CTRL_8_REG      0x37
#define SSD2119_GAMMA_CTRL_9_REG      0x3A
#define SSD2119_GAMMA_CTRL_10_REG     0x3B
#define SSD2119_V_RAM_POS_REG         0x44
#define SSD2119_H_RAM_START_REG       0x45
#define SSD2119_H_RAM_END_REG         0x46
#define SSD2119_X_RAM_ADDR_REG        0x4E
#define SSD2119_Y_RAM_ADDR_REG        0x4F

#define ENTRY_MODE_DEFAULT 0x6830
#define ENTRY_MODE_BMP 	   0x6810
#define MAKE_ENTRY_MODE(x) ((ENTRY_MODE_DEFAULT & 0xFF00) | (x))


/** @defgroup stm32f4_discovery_LCD_Private_FunctionPrototypes
  * @{
  */ 
#ifndef USE_Delay
static void delay(__IO uint32_t nCount);
#endif /* USE_Delay*/

/**
  * @}
  */ 

/**
  * @brief  LCD Default FSMC Init
  * @param  None
  * @retval None
  */
void LCD_DeInit(void)
{ 
	/* BANK 3 (of NOR/SRAM Bank 1~4) is disabled */
	__FSMC_NORSRAM_ENABLE(FSMC_Bank1, FSMC_NORSRAM_BANK3);
  
	/*!< LCD_SPI DeInit */
	FSMC_NORSRAM_DeInit(FSMC_Bank1, FSMC_Bank1E, FSMC_NORSRAM_BANK3);
   
	/*-- GPIO Configuration ------------------------------------------------------*/
	/* SRAM Data lines configuration */
	BSP_GPIO_PinCfg(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_NOPULL, 0, GPIO_AF0_MCO);
	BSP_GPIO_PinCfg(GPIOE, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_NOPULL, 0, GPIO_AF0_MCO);

	/* SRAM Address lines configuration */
	BSP_GPIO_PinCfg(GPIOF, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_NOPULL, 0, GPIO_AF0_MCO);
	BSP_GPIO_PinCfg(GPIOG, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_NOPULL, 0, GPIO_AF0_MCO);
	BSP_GPIO_PinCfg(GPIOD, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_MODE_INPUT, GPIO_NOPULL, 0, GPIO_AF0_MCO);

	/* NOE and NWE configuration */
	BSP_GPIO_PinCfg(GPIOD, GPIO_PIN_4 | GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_NOPULL, 0, GPIO_AF0_MCO);

	/* NE3 configuration */
	BSP_GPIO_PinCfg(GPIOG, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_NOPULL, 0, GPIO_AF0_MCO);

	/* NBL0, NBL1 configuration */
	BSP_GPIO_PinCfg(GPIOE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_MODE_INPUT, GPIO_NOPULL, 0, GPIO_AF0_MCO);
}

/**
  * @brief  Configures LCD Control lines (FSMC Pins) in alternate function mode.
  * @param  None
  * @retval None
  */
void LCD_CtrlLinesConfig(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	//__HAL_RCC_GPIOG_CLK_ENABLE();

	/*-- GPIO Configuration ------------------------------------------------------*/
	/* SRAM Data lines,  NOE and NWE configuration */
	BSP_GPIO_PinCfg(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_4 | GPIO_PIN_5, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF12_FSMC);
	BSP_GPIO_PinCfg(GPIOE, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF12_FSMC);

	/* SRAM Address lines configuration LCD-DC */
	BSP_GPIO_PinCfg(GPIOE, GPIO_PIN_3, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF12_FSMC);

	/* NE3 configuration */
	BSP_GPIO_PinCfg(GPIOD, GPIO_PIN_7, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF12_FSMC);

	/* LCD RST configuration */
	BSP_GPIO_PinCfg(LCD_RST_PORT, LCD_RST_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_LOW, GPIO_AF12_FSMC);
	BSP_GPIO_PinCfg(LCD_PWM_PORT, LCD_PWM_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_MEDIUM, GPIO_AF12_FSMC);


	HAL_GPIO_WritePin(LCD_PWM_PORT, LCD_PWM_PIN, GPIO_PIN_SET);
}

/**
  * @brief  Configures the Parallel interface (FSMC) for LCD(Parallel mode)
  * @param  None
  * @retval None
  */
void LCD_FSMCConfig(void)
{
	FSMC_NORSRAM_InitTypeDef FSMC_NORSRAM_InitStructure;
	FSMC_NORSRAM_TimingTypeDef p;
   
	/* Enable FSMC clock */
	__HAL_RCC_FSMC_CLK_ENABLE();
  
	/*-- FSMC Configuration ------------------------------------------------------*/
	/*----------------------- SRAM Bank 1 ----------------------------------------*/
	/* FSMC_Bank1_NORSRAM4 configuration */
	p.AddressSetupTime = 1;
	p.AddressHoldTime = 0;
	p.DataSetupTime = 9;
	p.BusTurnAroundDuration = 0;
	p.CLKDivision = 0;
	p.DataLatency = 0;
	p.AccessMode = FSMC_ACCESS_MODE_A;
  
	if(FSMC_NORSRAM_Timing_Init(FSMC_Bank1, &p, FSMC_NORSRAM_BANK1) != HAL_OK)
	{
		// Erreur à gérer
	}
	/* Color LCD configuration ------------------------------------
     LCD configured as follow:
        - Data/Address MUX = Disable
        - Memory Type = SRAM
        - Data Width = 16bit
        - Write Operation = Enable
        - Extended Mode = Enable
        - Asynchronous Wait = Disable */

    FSMC_NORSRAM_InitStructure.NSBank = FSMC_NORSRAM_BANK1;
	FSMC_NORSRAM_InitStructure.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
	FSMC_NORSRAM_InitStructure.MemoryType = FSMC_MEMORY_TYPE_SRAM;
	FSMC_NORSRAM_InitStructure.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
	FSMC_NORSRAM_InitStructure.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
	FSMC_NORSRAM_InitStructure.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
	FSMC_NORSRAM_InitStructure.WrapMode = FSMC_WRAP_MODE_DISABLE;
	FSMC_NORSRAM_InitStructure.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
	FSMC_NORSRAM_InitStructure.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
	FSMC_NORSRAM_InitStructure.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
	FSMC_NORSRAM_InitStructure.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
	FSMC_NORSRAM_InitStructure.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
	FSMC_NORSRAM_InitStructure.WriteBurst = FSMC_WRITE_BURST_DISABLE;
	
	if(FSMC_NORSRAM_Init(FSMC_Bank1, &FSMC_NORSRAM_InitStructure) != HAL_OK)
	{
		// Erreur à gérer
	}

	/* Enable FSMC NOR/SRAM Bank1 */
	__FSMC_NORSRAM_ENABLE(FSMC_Bank1, FSMC_NORSRAM_BANK1);
}

/**
  * @brief  LCD Init.
  * @retval None
  */
void STM32f4_Discovery_LCD_Init(void)
{ 
	unsigned long ulCount;
	
	/* Configure the LCD Control pins */
	LCD_CtrlLinesConfig();
	
	/* Configure the FSMC Parallel interface */
	LCD_FSMCConfig();
	
	_delay_(5);
	
	/* Reset LCD */
	//GPIO_ResetBits(LCD_RST_PORT, LCD_RST_PIN);
	HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);
	_delay_(10);
	//GPIO_SetBits(LCD_RST_PORT, LCD_RST_PIN);
	HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);

	/*
  	  SSD2119Init(void)
	 */
	/* Enter sleep mode (if we are not already there).*/
	LCD_WriteReg(SSD2119_SLEEP_MODE_1_REG, 0x0001);

	/* Set initial power parameters. */
	LCD_WriteReg(SSD2119_PWR_CTRL_5_REG, 0x00B2);
	LCD_WriteReg(SSD2119_VCOM_OTP_1_REG, 0x0006);
  
	/* Start the oscillator.*/
	LCD_WriteReg(SSD2119_OSC_START_REG, 0x0001);

	/* Set pixel format and basic display orientation (scanning direction).*/
	LCD_WriteReg(SSD2119_OUTPUT_CTRL_REG, 0x30EF);
	LCD_WriteReg(SSD2119_LCD_DRIVE_AC_CTRL_REG, 0x0600);

	/* Exit sleep mode.*/
	LCD_WriteReg(SSD2119_SLEEP_MODE_1_REG, 0x0000);
	_delay_(5);

	/* Configure pixel color format and MCU interface parameters.*/
	LCD_WriteReg(SSD2119_ENTRY_MODE_REG, ENTRY_MODE_DEFAULT);

	/* Set analog parameters */
	LCD_WriteReg(SSD2119_SLEEP_MODE_2_REG, 0x0999);
	LCD_WriteReg(SSD2119_ANALOG_SET_REG, 0x3800);

	/* Enable the display */
	LCD_WriteReg(SSD2119_DISPLAY_CTRL_REG, 0x0033);

	/* Set VCIX2 voltage to 6.1V.*/
	LCD_WriteReg(SSD2119_PWR_CTRL_2_REG, 0x0005);

	/* Configure gamma correction.*/
	LCD_WriteReg(SSD2119_GAMMA_CTRL_1_REG, 0x0000);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_2_REG, 0x0303);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_3_REG, 0x0407);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_4_REG, 0x0301);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_5_REG, 0x0301);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_6_REG, 0x0403);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_7_REG, 0x0707);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_8_REG, 0x0400);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_9_REG, 0x0a00);
	LCD_WriteReg(SSD2119_GAMMA_CTRL_10_REG, 0x1000);

	/* Configure Vlcd63 and VCOMl */
	LCD_WriteReg(SSD2119_PWR_CTRL_3_REG, 0x000A);
	LCD_WriteReg(SSD2119_PWR_CTRL_4_REG, 0x2E00);

	/* Set the display size and ensure that the GRAM window is set to allow
     access to the full display buffer.*/
	LCD_WriteReg(SSD2119_V_RAM_POS_REG, (LCD_PIXEL_HEIGHT-1) << 8);
	LCD_WriteReg(SSD2119_H_RAM_START_REG, 0x0000);
	LCD_WriteReg(SSD2119_H_RAM_END_REG, LCD_PIXEL_WIDTH-1);

	LCD_WriteReg(SSD2119_X_RAM_ADDR_REG, 0x00);
	LCD_WriteReg(SSD2119_Y_RAM_ADDR_REG, 0x00);
  
	/* clear the lcd  */
	LCD_WriteReg(SSD2119_RAM_DATA_REG, 0x0000);
	for(ulCount = 0; ulCount < (LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT); ulCount++)
	{
		LCD_WriteRAM(0x0000);
	}
	//LCD_SetFont(&LCD_DEFAULT_FONT);
}


/**
  * @brief  Sets the cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position. 
  * @retval None
  */
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	/* Set the X address of the display cursor.*/
	LCD_WriteReg(SSD2119_X_RAM_ADDR_REG, Xpos);
	
	/* Set the Y address of the display cursor.*/
	LCD_WriteReg(SSD2119_Y_RAM_ADDR_REG, Ypos);
}

/**
  * @brief  Writes to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @param  LCD_RegValue: value to write to the selected register.
  * @retval None
  */
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
	/* Write 16-bit Index, then Write Reg */
	LCD_CMD = LCD_Reg;
	/* Write 16-bit Reg */
	LCD_Data = LCD_RegValue;
}

/**
  * @brief  Reads the selected LCD Register.
  * @param  LCD_Reg: address of the selected register.
  * @retval LCD Register Value.
  */
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
	/* Write 16-bit Index (then Read Reg) */
	LCD_CMD = LCD_Reg;
	/* Read 16-bit Reg */
	return (LCD_Data);
}

/**
  * @brief  Prepare to write to the LCD RAM.
  * @param  None
  * @retval None
  */
void LCD_WriteRAM_Prepare(void)
{
	LCD_CMD = SSD2119_RAM_DATA_REG;
}

/**
  * @brief  Writes to the LCD RAM.
  * @param  RGB_Code: the pixel color in RGB mode (5-6-5).
  * @retval None
  */
void LCD_WriteRAM(uint16_t RGB_Code)
{
	/* Write 16-bit GRAM Reg */
	LCD_Data = RGB_Code;
}

/**
  * @brief  Reads the LCD RAM.
  * @param  None
  * @retval LCD RAM Value.
  */
uint16_t LCD_ReadRAM(void)
{
	/* Write 16-bit Index (then Read Reg) */
	//  LCD_CMD = SSD2119_RAM_DATA_REG; /* Select GRAM Reg */
	/* Read 16-bit Reg */
	return LCD_Data;
}

/**
  * @brief  Test LCD Display
  * @retval None
  */
void LCD_RGB_Test(void)
{
	uint32_t index;

	LCD_SetCursor(0x00, 0x00);
	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

	/* R */
	for(index = 0; index < (LCD_PIXEL_HEIGHT*LCD_PIXEL_WIDTH)/3; index++)
	{
		LCD_Data = LCD_COLOR_MAGENTA;
	}
	  
	/* G */
	for(;index < 2*(LCD_PIXEL_HEIGHT*LCD_PIXEL_WIDTH)/3; index++)
	{
		LCD_Data = LCD_COLOR_GREEN;
	}
	  
	/* B */
	for(; index < LCD_PIXEL_HEIGHT*LCD_PIXEL_WIDTH; index++)
	{
		LCD_Data = LCD_COLOR_BLUE;
	}
}

void create_rectangle(uint16_t Xpos, uint16_t Ypos, uint16_t width, uint16_t height, uint16_t RGB_Code){

	uint32_t i,j;
	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

	for( i = 0; i < height; i++)
	{
		LCD_SetCursor(Xpos,Ypos);
		LCD_WriteRAM_Prepare();		//a mettre obligatoirement pour avoir un screen update du cursor
		for( j = 0; j < width; j++)
		{
			LCD_Data = RGB_Code;
		}

		Ypos++;

	}

}

void show_keyboard(){

	LCD_SetFont(&Font16x24);

	uint32_t i,j;

	int plus = 0;
	int offsetX = 60, offsetY = 50;
	char inc[10];

	for( i = 0; i < 3; i++){

		for( j = 0; j < 3; j++){
			plus++;
			sprintf(inc,"%d",plus);
			LCD_DrawCircle((j*100)+offsetX,((i*70))+offsetY,25,0xFFFF);
			LCD_DisplayStringLine((i*70)+40,((j*100))+52,(uint8_t*)inc,0xffff,0x2104,LCD_NO_DISPLAY_ON_UART);
		}
	}
}



/**
  * @brief  Sets a display window
  * @param  Xpos: specifies the X bottom left position.
  * @param  Ypos: specifies the Y bottom left position.
  * @param  Height: display window width.
  * @param  Width: display window Height.
  * @retval None
  */
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t width, uint16_t Height)
{
	uint32_t value = 0;

	LCD_WriteReg(SSD2119_H_RAM_START_REG, Xpos);

	if ((Xpos+width) >= LCD_PIXEL_WIDTH) {
		LCD_WriteReg(SSD2119_H_RAM_END_REG, LCD_PIXEL_WIDTH-1);
	} else {
		LCD_WriteReg(SSD2119_H_RAM_END_REG, Xpos+width);
	}

	if ((Ypos+Height) >= LCD_PIXEL_HEIGHT) {
		value = (LCD_PIXEL_HEIGHT-1) << 8;
	} else {
		value = (Ypos+Height) << 8;
	}
	value |= Xpos;
	LCD_WriteReg(SSD2119_V_RAM_POS_REG, value);
	LCD_SetCursor(Xpos, Ypos);
}

void DISP_BMP(uint8_t *bitmap,uint32_t x,uint32_t y,uint32_t w,uint32_t h)
{
  uint32_t size = w * h;
  w--;
  h--;
  uint32_t *bmp_ptr = (uint16_t *)bitmap;
  uint32_t i;
  LCD_SetDisplayWindow(x,y,w,h);
 // LCD_SetCursor(0x00, 0x00);
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

  for(i=0;i<size;i++)
  {
	LCD_Data = *bmp_ptr++;
  }

  LCD_SetDisplayWindow(0,0,319,239);
}

/**
  * @brief  Disables LCD Window mode.
  * @param  None
  * @retval None
  */
void LCD_WindowModeDisable(void)
{
	LCD_SetDisplayWindow(239, 0x13F, 240, 320);
	LCD_WriteReg(SSD2119_PWR_CTRL_1_REG, 0x1018);
}


#ifndef USE_Delay
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void delay(__IO uint32_t nCount)
{
	__IO uint32_t index = 0;
	for(index = (10000 * nCount); index != 0; index--)
	{
	}
}
#endif /* USE_Delay*/
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */  

/*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/
