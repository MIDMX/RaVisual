/**
  ******************************************************************************
  * @file    stm324xg_discovery_lcd.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-September-2011
  * @brief   This file contains all the functions prototypes for the 
  *          stm324xg_discovery_lcd.c driver.
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
  * @file    stm32f4_discovery_lcd.h
  * @author  CMP Team
  * @version V1.0.0
  * @date    28-December-2012
  * @brief   This file contains all the functions prototypes for the 
  *          stm324xg_discovery_lcd.c driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4_DISCOVERY_LCD_H
#define __STM32F4_DISCOVERY_LCD_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "fonts.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32F4_DISCOVERY
  * @{
  */ 

/** @addtogroup STM32F4_DISCOVERY
  * @{
  */
    
/** @addtogroup STM32F4_DISCOVERY_LCD
  * @{
  */ 


/** @defgroup STM32F4_DISCOVERY_LCD_Exported_Types
  * @{
  */
typedef struct 
{
  int16_t X;
  int16_t Y;
} Point, * pPoint;   
/**
  * @}
  */ 




/** 
  * @brief The dimensions of the LCD panel.
  */
#define LCD_VERTICAL_MAX   240
#define LCD_HORIZONTAL_MAX 320

/** 
  * @brief Various definitions controlling coordinate space mapping and drawing
           direction in the four supported orientations.   
  */
#define PORTRAIT

#ifdef PORTRAIT
#define HORIZ_DIRECTION 0x28
#define VERT_DIRECTION 0x20
#define MAPPED_X(x, y) (319 - (y))
#define MAPPED_Y(x, y) (x)
#endif
#ifdef LANDSCAPE
#define HORIZ_DIRECTION 0x00
#define VERT_DIRECTION  0x08
#define MAPPED_X(x, y) (319 - (x))
#define MAPPED_Y(x, y) (239 - (y))
#endif
#ifdef PORTRAIT_FLIP
#define HORIZ_DIRECTION 0x18
#define VERT_DIRECTION 0x10
#define MAPPED_X(x, y) (y)
#define MAPPED_Y(x, y) (239 - (x))
#endif
#ifdef LANDSCAPE_FLIP
#define HORIZ_DIRECTION 0x30
#define VERT_DIRECTION  0x38
#define MAPPED_X(x, y) (x)
#define MAPPED_Y(x, y) (y)
#endif

/** @defgroup STM32F4_DISCOVERY_LCD_Exported_Constants
  * @{
  */ 

/**
 * @brief Uncomment the line below if you want to use user defined Delay function
 *        (for precise timing), otherwise default _delay_ function defined within
 *         this driver is used (less precise timing).  
 */
/* #define USE_Delay */

#ifdef USE_Delay
#include "main.h" 
  #define _delay_     Delay  /* !< User can provide more timing precise _delay_ function
                                   (with 10ms time base), using SysTick for example */
#else
  #define _delay_     delay      /* !< Default _delay_ function with less precise timing */
#endif

/** 
  * @brief  LCD color  
  */ 
#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0x7BEF//0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

#define White		         LCD_COLOR_WHITE
#define Black		         LCD_COLOR_BLACK
#define Red		         LCD_COLOR_RED
#define Blue		         LCD_COLOR_BLUE
#define Green		         LCD_COLOR_GREEN
#define Cyan                     LCD_COLOR_CYAN


typedef __IO uint16_t vu16;
typedef uint32_t u32;
/* Note: LCD /CS is NE1 - Bank 1 of NOR/SRAM Bank 1~4 */
#define  LCD_BASE_Data               ((u32)(0x60000000|0x00100000))
#define  LCD_BASE_Addr               ((u32)(0x60000000|0x00000000))
#define  LCD_CMD                     (*(vu16 *)LCD_BASE_Addr)
#define  LCD_Data                    (*(vu16 *)LCD_BASE_Data)


/** 
  * @brief  LCD Lines depending on the chosen fonts.  
  */
#define LCD_LINE_0               LINE(0)
#define LCD_LINE_1               LINE(1)
#define LCD_LINE_2               LINE(2)
#define LCD_LINE_3               LINE(3)
#define LCD_LINE_4               LINE(4)
#define LCD_LINE_5               LINE(5)
#define LCD_LINE_6               LINE(6)
#define LCD_LINE_7               LINE(7)
#define LCD_LINE_8               LINE(8)
#define LCD_LINE_9               LINE(9)
#define LCD_LINE_10              LINE(10)
#define LCD_LINE_11              LINE(11)
#define LCD_LINE_12              LINE(12)
#define LCD_LINE_13              LINE(13)
#define LCD_LINE_14              LINE(14)
#define LCD_LINE_15              LINE(15)
#define LCD_LINE_16              LINE(16)
#define LCD_LINE_17              LINE(17)
#define LCD_LINE_18              LINE(18)
#define LCD_LINE_19              LINE(19)
#define LCD_LINE_20              LINE(20)
#define LCD_LINE_21              LINE(21)
#define LCD_LINE_22              LINE(22)
#define LCD_LINE_23              LINE(23)
#define LCD_LINE_24              LINE(24)
#define LCD_LINE_25              LINE(25)
#define LCD_LINE_26              LINE(26)
#define LCD_LINE_27              LINE(27)
#define LCD_LINE_28              LINE(28)
#define LCD_LINE_29              LINE(29)

#define Line0                    LCD_LINE_0
#define Line1                    LCD_LINE_1
#define Line2                    LCD_LINE_2
#define Line3                    LCD_LINE_3
#define Line4                    LCD_LINE_4
#define Line5                    LCD_LINE_5
#define Line6                    LCD_LINE_6
#define Line7                    LCD_LINE_7
#define Line8                    LCD_LINE_8
#define Line9                    LCD_LINE_9

/** 
  * @brief LCD default font 
  */ 
#define LCD_DEFAULT_FONT         Font16x24

/** 
  * @brief  LCD Direction  
  */ 
#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001

/** 
  * @brief  LCD Size (Width and Height)  
  */ 
#define LCD_PIXEL_WIDTH          320
#define LCD_PIXEL_HEIGHT         240

/**
  * @}
  */ 

/** @defgroup STM32F4_DISCOVERY_LCD_Exported_Macros
  * @{
  */
#define ASSEMBLE_RGB(R ,G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3)) 
/**
  * @}
  */ 

/** @defgroup STM32F4_DISCOVERY_LCD_Exported_Functions
  * @{
  */ 
/** @defgroup  
  * @{
  */
void LCD_DeInit(void);   
void STM32f4_Discovery_LCD_Init(void);
void LCD_RGB_Test(void);
void create_rectangle(uint16_t Xpos, uint16_t Ypos, uint16_t width, uint16_t height, uint16_t RGB_Code);
void show_keyboard();

void LCD_Clear(uint16_t Color);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);

/**
  * @}
  */ 

/** @defgroup  
  * @{
  */ 
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t LCD_ReadReg(uint8_t LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
uint16_t LCD_ReadRAM(void);

/**
  * @}
  */ 

/** @defgroup
  * @{
  */ 
void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);
/**
  * @}
  */
/**
  * @}
  */    
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4_DISCOVERY_LCD_H */
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
