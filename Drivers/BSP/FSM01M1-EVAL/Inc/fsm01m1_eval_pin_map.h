/**
  ******************************************************************************
  * @file    steval_fsm01m1.h
  * @author  EMEA Power Application Lab
  * @brief   Header for steval_fsm01m1.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STEVAL_FSM01M1_H
#define STEVAL_FSM01M1_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


 /** @addtogroup BSP BSP
   * @{
   */

 /** @addtogroup STEVAL_FSM01M1 STEVAL_FSM01M1
   * @{
   */

 /* Exported types ------------------------------------------------------------*/

 /* Exported constants --------------------------------------------------------*/

/* USER LEDs */

//#define B1_USER_Pin					GPIO_PIN_13
//#define B1_USER_Port				GPIOC

//#define LD2_USER_Pin				GPIO_PIN_5
//#define LD2_USER_GPIO_Port			GPIOA

//#define D12_USER_LED_GREEN_Pin			GPIO_PIN_2
//#define D12_USER_LED_GREEN_GPIO_Port	GPIOC
//
//#define D13_USER_LED_RED_Pin			GPIO_PIN_3
//#define D13_USER_LED_RED_GPIO_Port		GPIOC

/* DIGITAL OUTPUT */
/* Channel 1 - output mapping */

/*
#define VCC1_CTRL_Pin				GPIO_PIN_11
#define VCC1_CTRL_GPIO_Port			GPIOA

#define VCC1_DSC_Pin				GPIO_PIN_12
#define VCC1_DSC_GPIO_Port			GPIOA

#define OUT1_CTRL_Pin				GPIO_PIN_11
#define OUT1_CTRL_GPIO_Port			GPIOC

#define OUT1_DSC_Pin				GPIO_PIN_10
#define OUT1_DSC_GPIO_Port			GPIOC

#define CUTOFF1_CTRL_Pin			GPIO_PIN_2
#define CUTOFF1_CTRL_GPIO_Port		GPIOD

#define OUT1_DIAG_Pin				GPIO_PIN_12
#define OUT1_DIAG_GPIO_Port			GPIOC
*/

/* Channel 2 - output mapping */
/*
#define VCC2_CTRL_Pin			GPIO_PIN_9
#define VCC2_CTRL_GPIO_Port			GPIOB

#define VCC2_DSC_Pin			GPIO_PIN_8
#define VCC2_DSC_GPIO_Port			GPIOB

#define OUT2_CTRL_Pin			GPIO_PIN_5
#define OUT2_CTRL_GPIO_Port			GPIOC

#define OUT2_DSC_Pin			GPIO_PIN_6
#define OUT2_DSC_GPIO_Port			GPIOC

#define CUTOFF2_CTRL_Pin		GPIO_PIN_8
#define CUTOFF2_CTRL_GPIO_Port		GPIOC

#define OUT2_DIAG_Pin			GPIO_PIN_9
#define OUT2_DIAG_GPIO_Port			GPIOC

*/

 /* DIGITAL INPUT */
 /* Channel 1 - input mapping */

/*
#define OUTP1_Pin				GPIO_PIN_0
#define OUTP1_GPIO_Port				GPIOC

#define TP1_CTRL_Pin			GPIO_PIN_15
#define TP1_CTRL_GPIO_Port			GPIOA
*/


/* Channel 2 - input mapping */

/*
#define OUTP2_Pin				GPIO_PIN_1
#define OUTP2_GPIO_Port				GPIOH

#define TP2_CTRL_Pin			GPIO_PIN_7
#define TP2_CTRL_GPIO_Port			GPIOB
*/


/* SPI interface mapping */

//#define SPI_NCS_Pin				GPIO_PIN_12
//#define SPI_NCS_GPIO_Port			GPIOB

//#define SPI_SCK_Pin				GPIO_PIN_13
//#define SPI_SCK_GPIO_Port			GPIOB

//#define SPI_MOSI_Pin			GPIO_PIN_15
//#define SPI_MOSI_GPIO_Port			GPIOB

//#define SPI_MISO_Pin			GPIO_PIN_14
//#define SPI_MISO_Port			GPIOB



 /* External variables --------------------------------------------------------*/


 /* Exported macros -----------------------------------------------------------*/

 #if  defined ( __GNUC__ )
   #ifndef __weak
     #define __weak   __attribute__((weak))
   #endif /* __weak */
 #endif /* __GNUC__ */

 /* Exported functions ------------------------------------------------------- */

// void BSP_IOLinkDevice_AttachErrorHandler(void (*callback)(void));



/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
  }
#endif

#endif /* STEVAL_FSM01M1_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
