/**
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2012 Embest Tech. Co., Ltd.</center></h2>
  * @file    STMPE811QTR.c
  * @author  CMP Team
  * @version V1.0.0
  * @date    28-December-2012
  * @brief   LCD touch screen controler driver      
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
#include "stmpe811.h"
#include "stm32f4xx_hal.h"
#include "macro_types.h"
#include "lcd_display_form_and_text.h"
#include "stm32f4_gpio.h"



/** @defgroup STM32F4xx_StdPeriph_Examples
  * @{
  */ 
#define TIMEOUT_MAX    0x3000 /*<! The value of the maximal timeout for I2C waiting loops */
/**
  * @}
  */ 


/** @defgroup STM32F4_DISCOVERY_IOE_Private_Variables
  * @{
  */ 
static TS_STATE TS_State;              /*<! The global structure holding the TS state */

static uint32_t IOE_TimeOut = TIMEOUT_MAX; /*<! Value of Timeout when I2C communication fails */
/**
  * @}
  */ 
static I2C_HandleTypeDef I2C_HandleStructure;
static DMA_HandleTypeDef  DMA_HandleStructure_tx;
static DMA_HandleTypeDef  DMA_HandleStructure_rx;


/** @defgroup STM32F4_DISCOVERY_IOE_Private_FunctionPrototypes
  * @{
  */ 
uint16_t IOE_TS_Read_Z(void);

static void IOE_GPIO_Config(void);
static void IOE_I2C_Config(void);
static void IOE_DMA_Config(IOE_DMADirection_TypeDef Direction/*, uint8_t* buffer*/);
static void IOE_EXTI_Config(void);


#ifndef USE_Delay
static void delay(__IO uint32_t nCount);
#endif /* USE_Delay*/
/**
  * @}
  */ 


/** @defgroup STM32F4_DISCOVERY_IOE_Private_Functions
  * @{
  */ 



/**
  * @brief  Initializes and Configures the two IO_Expanders Functionalities 
  *         (IOs, Touch Screen ..) and configures all STM32F4_DISCOVERY necessary
  *         hardware (GPIOs, APB clocks ..).
  * @param  None
  * @retval IOE_OK if all initializations done correctly. Other value if error.
  */
uint8_t IOE_Config(void)
{
  /* Configure the needed pins */
  IOE_GPIO_Config(); 
  
  IOE_I2C_Config();
    
  IOE_DMA_Config(IOE_DMA_RX);
  __HAL_LINKDMA(&I2C_HandleStructure, hdmarx, DMA_HandleStructure_rx);
  IOE_DMA_Config(IOE_DMA_TX);
  __HAL_LINKDMA(&I2C_HandleStructure, hdmatx, DMA_HandleStructure_tx);


  /* Read IO Expander 1 ID  */
  if (IOE_IsOperational(IOE_1_ADDR)) {
	   return IOE1_NOT_OPERATIONAL;
  }

  /* Generate IOExpander Software reset */
  IOE_Reset(IOE_1_ADDR);

  /* ---------------------- IO Expander 1 configuration --------------------- */
  /* Enable the GPIO, Touch Screen and ADC functionalities */
  IOE_FnctCmd(IOE_1_ADDR, IOE_IO_FCT | IOE_TS_FCT | IOE_ADC_FCT | IOE_TEMPSENS_FCT, ENABLE);

  /* Touch Screen controller configuration */
  IOE_TS_Config();

  /* Réglage du capteur de température */
  IOE_Temp_Config();

  /* Configuration is OK */
  return IOE_OK; 
}

/**
  * @brief  Configures The selected interrupts on the IO Expanders.
  * @param  IOE_ITSRC_Source: the source of the interrupts. Could be one or a 
  *         combination of the following parameters:
  *   @arg  IOE_ITSRC_JOYSTICK: Joystick IO intputs.
  *   @arg  IOE_ITSRC_TSC: Touch Screen interrupts.
  *   @arg  IOE_ITSRC_INMEMS: MEMS interrupt lines.
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_ITConfig(uint32_t IOE_ITSRC_Source)
{   
  /* Configure the Interrupt output pin to generate low level (INT_CTRL) */
  IOE_ITOutConfig(Polarity_High, Type_Level);  
  
  /* Manage the Touch Screen Interrupts */  
  if (IOE_ITSRC_Source & IOE_ITSRC_TSC) {   
    /* Enable the Global interrupt */  
    IOE_GITCmd(IOE_1_ADDR, ENABLE);     
           
    /* Enable the Global GPIO Interrupt */
    IOE_GITConfig(IOE_1_ADDR, (uint8_t)(IOE_GIT_TOUCH | IOE_GIT_FTH), ENABLE);    
    
    /* Read the GPIO_IT_STA to clear all pending bits if any */
    I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_GPIO_INT_STA); 
  }
  
  /* Configure the Interrupt line as EXTI source */
  IOE_EXTI_Config();    
  
  /* If all OK return IOE_OK */
  return IOE_OK;
}

/**
  * @brief  Returns Status and positions of the Touch screen.
  * @param  None
  * @retval Pointer to TS_STATE structure holding Touch Screen information.
  */
TS_STATE* IOE_TS_GetState(void)
{
	uint16_t x, y, z;
	uint8_t nb_try;
	/* Check if the Touch detect event happened */
	TS_State.TouchDetected = (uint16_t)((I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_CTRL) & 0x80) != 0);

	if (TS_State.TouchDetected == TRUE)
	{

		x = IOE_TS_Read_X();
		y = IOE_TS_Read_Y();
		z = IOE_TS_Read_Z();

		if(x == 0 || y == 0)
			TS_State.TouchDetected = FALSE;
		else
		{
			TS_State.X = x;
			TS_State.Y = y;
			TS_State.Z = z;
		}
	}
	
	/* Clear the interrupt pending bit and enable the FIFO again */
	nb_try = 0;
	do{
		nb_try++;
	}while(nb_try < 3 && I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_FIFO_STA, 0x01) == IOE_TIMEOUT);

	nb_try = 0;
	do{
		nb_try++;
	}while(nb_try < 3 && I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_FIFO_STA, 0x00) == IOE_TIMEOUT);


	/* Return pointer to the updated structure */
	return &TS_State;
}

/**
  * @brief  Checks the selected Global interrupt source pending bit
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  Global_IT: the Global interrupt source to be checked, could be:
  *   @arg  Global_IT_GPIO : All IOs interrupt
  *   @arg  Global_IT_ADC : ADC interrupt
  *   @arg  Global_IT_TEMP : Temperature Sensor interrupts      
  *   @arg  Global_IT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  Global_IT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  Global_IT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  Global_IT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  Global_IT_TOUCH : Touch Screen Controller Touch Detected interrupt      
  * @retval Status of the checked flag. Could be SET or RESET.
  */
FlagStatus IOE_GetGITStatus(uint8_t DeviceAddr, uint8_t Global_IT)
{
  __IO uint8_t tmp = 0;
 
  /* get the Interrupt status */
  tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_INT_STA);
  
  if ((tmp & (uint8_t)Global_IT) != 0) {
    return SET;
  } else {
    return RESET;
  }
}

/**
  * @brief  Clears the selected Global interrupt pending bit(s)
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  Global_IT: the Global interrupt to be cleared, could be any combination
  *         of the following values:   
  *   @arg  Global_IT_GPIO : All IOs interrupt
  *   @arg  Global_IT_ADC : ADC interrupt
  *   @arg  Global_IT_TEMP : Temperature Sensor interrupts      
  *   @arg  Global_IT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  Global_IT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  Global_IT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  Global_IT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  Global_IT_TOUCH : Touch Screen Controller Touch Detected interrupt 
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_ClearGITPending(uint8_t DeviceAddr, uint8_t Global_IT)
{
  /* Write 1 to the bits that have to be cleared */
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_INT_STA, Global_IT); 

  /* If all OK return IOE_OK */
  return IOE_OK;
}

/**
  * @brief  Checks the status of the selected IO interrupt pending bit
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_IT: the IO interrupt to be checked could be IO_ITx Where x can be 
  *         from 0 to 7.             
  * @retval Status of the checked flag. Could be SET or RESET.
  */
FlagStatus IOE_GetIOITStatus(uint8_t DeviceAddr, uint8_t IO_IT)
{
  uint8_t tmp = 0;
 
  /* get the Interrupt status */
  tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_INT_STA);
  
  if ((tmp & (uint8_t)IO_IT) != 0) {
    return SET;
  } else {
    return RESET;
  }
}

/**
  * @brief  Checks if the selected device is correctly configured and 
  *         communicates correctly on the I2C bus.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @retval IOE_OK if IOE is operational. Other value if failure.
  */
uint8_t IOE_IsOperational(uint8_t DeviceAddr)
{


  /* Return Error if the ID is not correct */
  if ( IOE_ReadID(DeviceAddr) != (uint16_t)STMPE811_ID ) {
    /* Check if a Timeout occurred */
    if (IOE_TimeOut == 0) {
      return (IOE_TimeoutUserCallback());
    } else {
      return IOE_FAILURE; /* ID is not Correct */
    }
  } else {
    return IOE_OK; /* ID is correct */
  }
}

/**
  * @brief  Resets the IO Expander by Software (SYS_CTRL1, RESET bit).
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_Reset(uint8_t DeviceAddr)
{
  /* Power Down the IO_Expander */
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_SYS_CTRL1, 0x02);
  /* wait for a delay to insure registers erasing */
  _delay_(2); 
  
  /* Power On the Codec after the power off => all registers are reinitialized*/
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_SYS_CTRL1, 0x00);
  /* If all OK return IOE_OK */
  return IOE_OK;    
}

/**
  * @brief  Reads the selected device's ID.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @retval The Device ID (two bytes).
  */
uint16_t IOE_ReadID(uint8_t DeviceAddr)
{
  uint16_t tmp = 0;

  /* Read device ID  */
  tmp = I2C_ReadDeviceRegister(DeviceAddr, 0);

  tmp = (uint32_t)(tmp << 8);

  tmp |= (uint32_t)I2C_ReadDeviceRegister(DeviceAddr, 1);

  /* Return the ID */
  return (uint16_t)tmp;
}

/**
  * @brief  Configures the selected IO Expander functionalities.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IOE_TEMPSENS_FCT: the functions to be configured. could be any 
  *         combination of the following values:
  *   @arg  IOE_IO_FCT : IO function
  *   @arg  IOE_TS_FCT : Touch Screen function
  *   @arg  IOE_ADC_FCT : ADC function
  *   @arg  IOE_TEMPSENS_FCT : Temperature Sensor function
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_FnctCmd(uint8_t DeviceAddr, uint8_t Fct, FunctionalState NewState)
{
  uint8_t tmp = 0;
  
  /* Get the register value */
  tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_SYS_CTRL2);
  
  if (NewState != DISABLE) {
    /* Set the Functionalities to be Enabled */    
    tmp &= ~(uint8_t)Fct;
  } else {
    /* Set the Functionalities to be Disabled */    
    tmp |= (uint8_t)Fct;  
  }
  
  /* Set the register value */
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_SYS_CTRL2, tmp);

  /* If all OK return IOE_OK */
  return IOE_OK;    
}

/**
  * @brief  Configures the selected pin direction (to be an input or an output)
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_Pin: IO_Pin_x: Where x can be from 0 to 7.   
  * @param  Direction: could be Direction_IN or Direction_OUT.      
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_IOPinConfig(uint8_t DeviceAddr, uint8_t IO_Pin, uint8_t Direction)
{
  uint8_t tmp = 0;   
  
  /* Get all the Pins direction */
  tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_DIR);
  
  if (Direction != Direction_IN) {
    tmp |= (uint8_t)IO_Pin;
  } else {
    tmp &= ~(uint8_t)IO_Pin;
  }
  
  /* Write the register new value */
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_DIR, tmp);
  
  /* If all OK return IOE_OK */
  return IOE_OK;      
}

/**
  * @brief  Enables or disables the Global interrupt.
  * @param  DeviceAddr: The address of the IOExpander, could be :I OE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  NewState: could be ENABLE or DISABLE.        
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_GITCmd(uint8_t DeviceAddr, FunctionalState NewState)
{
  uint8_t tmp = 0;
  
  /* Read the Interrupt Control register  */
  I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_INT_CTRL);
  
  if (NewState != DISABLE) {
    /* Set the global interrupts to be Enabled */    
    tmp |= (uint8_t)IOE_GIT_EN;
  } else {
    /* Set the global interrupts to be Disabled */    
    tmp &= ~(uint8_t)IOE_GIT_EN;
  }  
  
  /* Write Back the Interrupt Control register */
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_INT_CTRL, tmp);

  /* If all OK return IOE_OK */
  return IOE_OK;     
}

/**
  * @brief  Configures the selected source to generate or not a global interrupt
  * @param DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *        or IOE_2_ADDR.
  * @param Global_IT: the interrupt source to be configured, could be:
  *   @arg  Global_IT_GPIO : All IOs interrupt
  *   @arg  Global_IT_ADC : ADC interrupt
  *   @arg  Global_IT_TEMP : Temperature Sensor interrupts      
  *   @arg  Global_IT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  Global_IT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  Global_IT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  Global_IT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  Global_IT_TOUCH : Touch Screen Controller Touch Detected interrupt 
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_GITConfig(uint8_t DeviceAddr, uint8_t Global_IT, FunctionalState NewState)
{
  uint8_t tmp = 0;
  
  /* Get the current value of the INT_EN register */
  tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_INT_EN);
  
  if (NewState != DISABLE) {
    /* Set the interrupts to be Enabled */    
    tmp |= (uint8_t)Global_IT;  
  } else {
    /* Set the interrupts to be Disabled */    
    tmp &= ~(uint8_t)Global_IT;
  }
  /* Set the register */
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_INT_EN, tmp);
  
  /* If all OK return IOE_OK */
  return IOE_OK;  
}

/**
  * @brief  Configures the selected pins to generate an interrupt or not.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_IT: The IO interrupt to be configured. This parameter could be any
  *         combination of the following values:
  *   @arg  IO_IT_x: where x can be from 0 to 7.
  * @param  NewState: could be ENABLE or DISABLE.  
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_IOITConfig(uint8_t DeviceAddr, uint8_t IO_IT, FunctionalState NewState)
{
  uint8_t tmp = 0;
 
  tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_INT_EN);
  
  if (NewState != DISABLE) {
    /* Set the interrupts to be Enabled */    
    tmp |= (uint8_t)IO_IT;
  } else {
    /* Set the interrupts to be Disabled */    
    tmp &= ~(uint8_t)IO_IT;
  }
  
  /* Set the register */
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_INT_EN, tmp);
  
  /* If all OK return IOE_OK */
  return IOE_OK;   
}

/**
  * @brief  Configures the touch Screen Controller (Single point detection)
  * @param  None
  * @retval IOE_OK if all initializations are OK. Other value if error.
  */
uint8_t IOE_TS_Config(void)
{
	uint8_t tmp = 0;

	/* Enable TSC Fct: already done in IOE_Config */
	tmp = I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_SYS_CTRL2);
	tmp &= ~(uint32_t)(IOE_TS_FCT | IOE_ADC_FCT);
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_SYS_CTRL2, tmp);

	/* Enable the TSC global interrupts */
	tmp = I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_INT_EN);
	tmp |= (uint32_t)(IOE_GIT_TOUCH | IOE_GIT_FTH );
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_INT_EN, tmp);

	/* Wait for ~20 ms */
	_delay_(2);

	/* Select the ADC clock speed: 3.25 MHz */
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_ADC_CTRL2, 0x01);

	/* Select TSC pins in non default mode */
	tmp = I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_GPIO_AF);
	tmp &= ~(uint8_t)TOUCH_IO_ALL;
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_GPIO_AF, tmp);

	/* set the data format for Z value: 7 fractional part and 1 whole part */
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_FRACT_XYZ, 0x01);

	/* set the driving capability of the device for TSC pins: 20mA */
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_I_DRIVE, 0x00);

	/* Use no tracking index, touchscreen controller operation mode (XY)*/
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_CTRL, 0x02);

	/* Select filter capacitor */
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_CFG, 0xD0);

	/* Select Sample Time, bit number and ADC Reference */
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_ADC_CTRL1, 0x08);

	/* Select single point reading  */
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_FIFO_TH, 0x01);

	/* Write 0x01 to clear the FIFO memory content. */
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_FIFO_STA, 0x01);

	/* Write 0x00 to put the FIFO back into operation mode  */
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_FIFO_STA, 0x00);

	/*enable the TSC */
	tmp = I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_CTRL);
	tmp |= (uint32_t)0x1;
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_TSC_CTRL, tmp);

	/*  Clear all the status pending bits */
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_INT_STA, 0xFF);

	/* Initialize the TS structure to their default values */
	TS_State.TouchDetected = TS_State.X = TS_State.Y = TS_State.Z = 0;
  
  /* All configuration done */
  return IOE_OK;  
}

/**
  * @brief  Configures the selected pin to be in Alternate function or not
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_Pin: IO_Pin_x, Where x can be from 0 to 7.   
  * @param  NewState: State of the AF for the selected pin, could be 
  *         ENABLE or DISABLE.       
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_IOAFConfig(uint8_t DeviceAddr, uint8_t IO_Pin, FunctionalState NewState)
{
  uint8_t tmp = 0;
  
  /* Get the current state of the GPIO_AF register */
  tmp = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_AF);
  
  if (NewState != DISABLE) {
    /* Enable the selected pins alternate function */
    tmp |= (uint8_t)IO_Pin;
  } else {
    /* Disable the selected pins alternate function */   
    tmp &= ~(uint8_t)IO_Pin;   
  }
  
  /* Write back the new value in GPIO_AF register */
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_AF, tmp);  

  /* If all OK return IOE_OK */
  return IOE_OK;
}

/**
  * @brief  Configures the Edge for which a transition is detectable for the
  *         the selected pin.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR.
  * @param  IO_Pin: IO_Pin_x, Where x can be from 0 to 7.   
  * @param  Edge: The edge which will be detected. This parameter can be one or a
  *         a combination of following values: EDGE_FALLING and EDGE_RISING .
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_IOEdgeConfig(uint8_t DeviceAddr, uint8_t IO_Pin, uint8_t Edge)
{
  uint8_t tmp1 = 0, tmp2 = 0;   
  
  /* Get the registers values */
  tmp1 = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_FE);
  tmp2 = I2C_ReadDeviceRegister(DeviceAddr, IOE_REG_GPIO_RE);

  /* Disable the Falling Edge */
  tmp1 &= ~(uint8_t)IO_Pin;
  /* Disable the Falling Edge */
  tmp2 &= ~(uint8_t)IO_Pin;

  /* Enable the Falling edge if selected */
  if (Edge & EDGE_FALLING) {
    tmp1 |= (uint8_t)IO_Pin;
  }

  /* Enable the Rising edge if selected */
  if (Edge & EDGE_RISING) {
    tmp2 |= (uint8_t)IO_Pin;
  }

  /* Write back the registers values */
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_FE, tmp1);
  I2C_WriteDeviceRegister(DeviceAddr, IOE_REG_GPIO_RE, tmp2);
  
  /* if OK return 0 */
  return IOE_OK;
}

/**
  * @brief  Configures the Interrupt line active state and format (level/edge)
  * @param  Polarity: could be
  *   @arg  Polarity_Low: Interrupt line is active Low/Falling edge      
  *   @arg  Polarity_High: Interrupt line is active High/Rising edge      
  * @param  Type: Interrupt line activity type, could be one of the following values
  *   @arg  Type_Level: Interrupt line is active in level model         
  *   @arg  Type_Edge: Interrupt line is active in edge model           
  * @retval IOE_OK: if all initializations are OK. Other value if error.
  */
uint8_t IOE_ITOutConfig(uint8_t Polarity, uint8_t Type)
{
  uint8_t tmp = 0;
  
  /*  Get the register IOE_REG_INT_CTRL value */ 
  tmp = I2C_ReadDeviceRegister(IOE_1_ADDR, IOE_REG_INT_CTRL);
  
  /* Mask the polarity and type bits */
  tmp &= ~(uint8_t)0x06;
    
  /* Modify the Interrupt Output line configuration */
  tmp |= (uint8_t)(Polarity | Type);
  
  /* Set the register */
  I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_INT_CTRL, tmp); 
  
  /* If all OK return IOE_OK */
  return IOE_OK;  
}

/**
  * @brief  Writes a value in a register of the device through I2C.
  * @param  DeviceAddr: The address of the IOExpander, could be : IOE_1_ADDR
  *         or IOE_2_ADDR. 
  * @param  RegisterAddr: The target register address
  * @param  RegisterValue: The target register value to be written 
  * @retval IOE_OK: if all operations are OK. Other value if error.
  */
uint8_t I2C_WriteDeviceRegister(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t RegisterValue)
{
	static uint8_t read_verif = 0;
	static uint8_t IOE_BufferTX = 0;

	/* Get Value to be written */
	IOE_BufferTX = RegisterValue;

	if(HAL_I2C_GetState(&I2C_HandleStructure) == HAL_I2C_STATE_READY)
	{
		if(HAL_DMA_Start(&DMA_HandleStructure_tx, (uint32_t)(&IOE_BufferTX), IOE_I2C_DR, 1) != HAL_OK)
		{
			// Erreur à gérer
		}

		while(HAL_I2C_Mem_Write_DMA(&I2C_HandleStructure, (uint16_t)DeviceAddr, (uint16_t)RegisterAddr, (uint16_t)I2C_MEMADD_SIZE_8BIT, &IOE_BufferTX, 1) != HAL_OK)
		{
			if(HAL_I2C_GetError(&I2C_HandleStructure) != HAL_I2C_ERROR_AF)
			{

				//Error_Handler();
			}
			goto error_case;
		}

		IOE_TimeOut = 2 * TIMEOUT_MAX;
		while (__HAL_DMA_GET_FLAG(I2C_HandleStructure.hdmatx, IOE_DMA_TX_TCFLAG) == RESET)
		{
			if (IOE_TimeOut-- == 0) {
				return(IOE_TimeoutUserCallback());
			}
		}
		__HAL_DMA_CLEAR_FLAG(I2C_HandleStructure.hdmatx, IOE_DMA_TX_TCFLAG);

		I2C_HandleStructure.Instance->CR2 &= (uint16_t)~((uint16_t)I2C_CR2_DMAEN);

    	IOE_TimeOut = 2 * TIMEOUT_MAX;
		while (__HAL_I2C_GET_FLAG(&I2C_HandleStructure, I2C_FLAG_BTF) == RESET)
		{
			if (IOE_TimeOut-- == 0) {
				goto error_case;
			}
		}

  	    I2C_HandleStructure.Instance->CR1 |= I2C_CR1_STOP;
  	  	HAL_DMA_Abort(&DMA_HandleStructure_tx);
  	    I2C_HandleStructure.State = HAL_I2C_STATE_READY;
	}
  
#ifdef VERIFY_WRITTENDATA
  /* Verify (if needed) that the loaded data is correct  */
  
  /* Read the just written register*/
  read_verif = I2C_ReadDeviceRegister(DeviceAddr, RegisterAddr);
  /* Load the register and verify its value  */
  if (read_verif != RegisterValue) {
    /* Control data wrongly transfered */
    read_verif = IOE_FAILURE;
  } else {
    /* Control data correctly transfered */
    read_verif = 0;
  }
#endif
  
  /* Return the verifying value: 0 (Passed) or 1 (Failed) */
  return (read_verif);

error_case:
	return IOE_TimeoutUserCallback();
}

/**
  * @brief  Reads a register of the device through I2C.
  * @param  DeviceAddr: The address of the device, could be : IOE_1_ADDR
  *         or IOE_2_ADDR. 
  * @param  RegisterAddr: The target register address (between 00x and 0x24)
  * @retval The value of the read register (0xAA if Timeout occurred)
  */
uint8_t I2C_ReadDeviceRegister(uint8_t DeviceAddr, uint8_t RegisterAddr)
{
	static uint8_t IOE_BufferRX[2] = {0x00, 0x00};
  
  	if (HAL_I2C_GetState(&I2C_HandleStructure) == HAL_I2C_STATE_READY)
	{
  		if(HAL_DMA_Start(&DMA_HandleStructure_rx, IOE_I2C_DR, (uint32_t)&IOE_BufferRX, 2) != HAL_OK)
  		{
  			// Erreur à gérer
  		}
		if(HAL_I2C_Mem_Read_DMA(&I2C_HandleStructure, (uint16_t)DeviceAddr, (uint16_t)RegisterAddr, (uint16_t)I2C_MEMADD_SIZE_8BIT, IOE_BufferRX, 2) != HAL_OK)
		{
			if (HAL_I2C_GetError(&I2C_HandleStructure) != HAL_I2C_ERROR_AF)
			{
				// Erreur à gérer
			}
		}


		IOE_TimeOut = 2 * TIMEOUT_MAX;

		while (__HAL_DMA_GET_FLAG(I2C_HandleStructure.hdmarx, IOE_DMA_RX_TCFLAG) == RESET)
		{
			  if (IOE_TimeOut-- == 0) {
				  return(IOE_TimeoutUserCallback());
			  }
		}
		__HAL_DMA_CLEAR_FLAG(I2C_HandleStructure.hdmarx, IOE_DMA_RX_TCFLAG);
		I2C_HandleStructure.Instance->CR1 |= I2C_CR1_STOP;
		I2C_HandleStructure.Instance->CR2 &= (uint16_t)~((uint16_t)I2C_CR2_DMAEN);

		HAL_DMA_Abort(&DMA_HandleStructure_rx);
		I2C_HandleStructure.State = HAL_I2C_STATE_READY;
	}
  	return (uint8_t)IOE_BufferRX[0];
}

/**
  * @brief  Reads a buffer of 2 bytes from the device registers.
  * @param  DeviceAddr: The address of the device, could be : IOE_1_ADDR
  *         or IOE_2_ADDR. 
  * @param  RegisterAddr: The target register address (between 00x and 0x24)
  * @retval A pointer to the buffer containing the two returned bytes (in halfword).  
  */
uint16_t I2C_ReadDataBuffer(uint8_t DeviceAddr, uint8_t RegisterAddr)
{ 
  static uint8_t IOE_BufferRX[2] = {0x00, 0x00};
  
  IOE_BufferRX[0] = I2C_ReadDeviceRegister(DeviceAddr, RegisterAddr+1);
  IOE_BufferRX[1] = I2C_ReadDeviceRegister(DeviceAddr, RegisterAddr);

  /* return a pointer to the IOE_Buffer */
  return (*(__IO uint16_t *) IOE_BufferRX);
}

/**
  * @brief  Return Touch Screen X position value
  * @param  None
  * @retval X position.
  */
uint16_t IOE_TS_Read_X(void)
{
	return I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_X);
}

/**
  * @brief  Return Touch Screen Y position value
  * @param  None
  * @retval Y position.
  */
uint16_t IOE_TS_Read_Y(void)
{
	return I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_Y);
}

/**
  * @brief  Return Touch Screen Z position value
  * @param  None
  * @retval Z position.
  */
uint16_t IOE_TS_Read_Z(void)
{
	return I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_Z);
}

uint8_t IOE_TimeoutUserCallback(void)
{
	//IOE_GPIO_Config();
	//I2C_DeInit(IOE_I2C);
	HAL_I2C_DeInit (&I2C_HandleStructure);
	IOE_I2C_Config();
	//IOE_Config();
	return IOE_TIMEOUT;
}

void Check_I2C_Bus_not_busy(void)
{
	volatile Uint32 i;
	Uint8 loop;

	BSP_GPIO_PinCfg(IOE_I2C_SCL_GPIO_PORT, IOE_I2C_SCL_PIN, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_MEDIUM, 0);
	BSP_GPIO_PinCfg(IOE_I2C_SDA_GPIO_PORT, IOE_I2C_SDA_PIN, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_MEDIUM, 0);

	if(HAL_GPIO_ReadPin(IOE_I2C_SCL_GPIO_PORT, IOE_I2C_SCL_PIN) == GPIO_PIN_RESET || HAL_GPIO_ReadPin(IOE_I2C_SDA_GPIO_PORT, IOE_I2C_SDA_PIN) == GPIO_PIN_RESET)
	{
		BSP_GPIO_PinCfg(GPIOB, IOE_I2C_SDA_PIN, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_MEDIUM, 0);
		BSP_GPIO_PinCfg(IOE_I2C_SCL_GPIO_PORT, IOE_I2C_SCL_PIN, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_SPEED_MEDIUM, 0);

		loop = 0;
		do
		{
			loop++;
			HAL_GPIO_WritePin(IOE_I2C_SCL_GPIO_PORT, IOE_I2C_SCL_PIN, GPIO_PIN_RESET);
			for(i=0;i<100000;i++);
			HAL_GPIO_WritePin(IOE_I2C_SCL_GPIO_PORT, IOE_I2C_SCL_PIN, GPIO_PIN_SET);
			for(i=0;i<100000;i++);
		}while(HAL_GPIO_ReadPin(IOE_I2C_SDA_GPIO_PORT, IOE_I2C_SDA_PIN) == GPIO_PIN_RESET || loop == 10);

		debug_printf("manuals clock pulses to recover I2C : %d\n",loop);
	}
}


/**
  * @brief  Initializes the GPIO pins used by the IO expander.
  * @param  None
  * @retval None
  */
static void IOE_GPIO_Config(void)
{
	Check_I2C_Bus_not_busy();

	/* Enable IOE_I2C and IOE_I2C_GPIO_PORT & Alternate Function clocks */
	__HAL_RCC_I2C1_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_SYSCFG_CLK_ENABLE();
  
	/* Reset IOE_I2C IP */
	__HAL_RCC_I2C1_FORCE_RESET();
  
	/* Release reset signal of IOE_I2C IP */
	__HAL_RCC_I2C1_RELEASE_RESET();
  
	/* IOE_I2C SCL and SDA pins configuration */
	BSP_GPIO_PinCfg(IOE_I2C_SCL_GPIO_PORT, IOE_I2C_SCL_PIN, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF4_I2C1);
	BSP_GPIO_PinCfg(IOE_I2C_SDA_GPIO_PORT, IOE_I2C_SDA_PIN, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF4_I2C1);
}




/**
  * @brief  Configure the I2C Peripheral used to communicate with IO_Expanders.
  * @param  None
  * @retval None
  */
static void IOE_I2C_Config(void)
{
	/* IOE_I2C configuration */
	I2C_HandleStructure.Instance = IOE_I2C;

  	/* Set the I2C structure parameters */
  	I2C_HandleStructure.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  	I2C_HandleStructure.Init.DutyCycle = I2C_DUTYCYCLE_2;
  	I2C_HandleStructure.Init.OwnAddress1 = 0x00;
  	I2C_HandleStructure.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  	I2C_HandleStructure.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  	I2C_HandleStructure.Init.ClockSpeed = I2C_SPEED1;
  	I2C_HandleStructure.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  	I2C_HandleStructure.Init.OwnAddress2 = 0xFE;

    /* Initialize the I2C peripheral w/ selected parameters */
  	if(HAL_I2C_Init(&I2C_HandleStructure) != HAL_OK)
  	{
  		// Erreur à gérer
  	}
}

/**
  * @brief  Configure the DMA Peripheral used to handle communication via I2C.
  * @param  None
  * @retval None
  */

static void IOE_DMA_Config(IOE_DMADirection_TypeDef Direction/*, uint8_t* buffer*/)
{
	__HAL_RCC_DMA1_CLK_ENABLE();

  	if(Direction == IOE_DMA_RX) /* If using DMA for Reception */
	{
  		DMA_HandleStructure_rx.Init.Channel = IOE_DMA_CHANNEL;
		DMA_HandleStructure_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		DMA_HandleStructure_rx.Init.MemInc = DMA_MINC_ENABLE;
		DMA_HandleStructure_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		DMA_HandleStructure_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		DMA_HandleStructure_rx.Init.Mode = DMA_NORMAL;
		DMA_HandleStructure_rx.Init.Priority = DMA_PRIORITY_LOW;
		DMA_HandleStructure_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
		DMA_HandleStructure_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
		DMA_HandleStructure_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		DMA_HandleStructure_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
  		DMA_HandleStructure_rx.Instance = IOE_DMA_RX_STREAM;
		DMA_HandleStructure_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		if(HAL_DMA_DeInit(&DMA_HandleStructure_rx) != HAL_OK)
		{
			// Erreur à gérer
		}
		if(HAL_DMA_Init(&DMA_HandleStructure_rx) != HAL_OK)
		{
			// Erreur à gérer
		}
	}
	else if (Direction == IOE_DMA_TX) /* If using DMA for Transmission */
	{
		DMA_HandleStructure_tx.Init.Channel = IOE_DMA_CHANNEL;
		DMA_HandleStructure_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		DMA_HandleStructure_tx.Init.MemInc = DMA_MINC_ENABLE;
		DMA_HandleStructure_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		DMA_HandleStructure_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		DMA_HandleStructure_tx.Init.Mode = DMA_NORMAL;
		DMA_HandleStructure_tx.Init.Priority = DMA_PRIORITY_LOW;
		DMA_HandleStructure_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
		DMA_HandleStructure_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
		DMA_HandleStructure_tx.Init.MemBurst = DMA_MBURST_SINGLE;
		DMA_HandleStructure_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		DMA_HandleStructure_tx.Instance = IOE_DMA_TX_STREAM;
		DMA_HandleStructure_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		if(HAL_DMA_DeInit(&DMA_HandleStructure_tx) != HAL_OK)
		{
			// Erreur à gérer
		}
		if(HAL_DMA_Init(&DMA_HandleStructure_tx) != HAL_OK)
		{
			// Erreur à gérer
		}
	}
}

/**
  * @brief  Configures the IO expander Interrupt line and GPIO in EXTI mode.
  * @param  None        
  * @retval None
  */
static void IOE_EXTI_Config(void)
{
#if 0
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
 
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(IOE_IT_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure Button pin as input floating */
  GPIO_InitStructure.GPIO_Pin = IOE_IT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IOE_IT_GPIO_PORT, &GPIO_InitStructure);  
  
  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(IOE_IT_EXTI_PORT_SOURCE, IOE_IT_EXTI_PIN_SOURCE);  
  
  /* Configure Button EXTI line */
  EXTI_InitStructure.EXTI_Line = IOE_IT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = IOE_IT_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
}

uint16_t IOE_Get_Temp(TS_temp_unit_e unit)
{
	#define VIO 2.9
	float temperature = (float)I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TEMP_DATA);
	temperature = (VIO*temperature / 7.51)-273.15;
	if (unit == TS_TEMP_UNIT_FARENHEIT)
		temperature = 1.8*temperature + 32;

	return (uint16_t) temperature*(temperature>=0);
}

void IOE_Temp_Config(void)
{
	/* Horloge du capteur de temperature déjà init dans IOE_Config()*/
	/* Interruption sur seuil désactivée,acquisition toute les 10ms, mesure temp activée */
	I2C_WriteDeviceRegister(IOE_1_ADDR, IOE_REG_TEMP_CTRL, 0x07);
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
  for(index = (100000 * nCount); index != 0; index--)
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
/******************** COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/
