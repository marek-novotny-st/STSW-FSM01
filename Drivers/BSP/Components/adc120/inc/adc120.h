/**
 ******************************************************************************
 * @file    adc120.h
 * @author  ST Power Application Lab Prague
 * @version V1.0.0
 * @date    Nov 18, 2021
 * @brief   header for ADC120 driver (8-channel 12-bit A/D converter)
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BSP_COMPONENTS_ADC120_INC_ADC120_H_
#define BSP_COMPONENTS_ADC120_INC_ADC120_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include <fsm01m1_eval_pin_map.h>


/* Exported Constants --------------------------------------------------------*/

/* adc 12b data extraction mask */
#define ADC120_DATA_MASK	(0x0FFF)

#define ADC120_CHANNELS_COUNT			(8)
#define ADC120_INVALID_CHANNEL_ID 		(0xFF)
#define ADC120_DEFAULT_CHANNEL_ID		(0x00)

#define ADC120_BIT_WIDTH				(12d)
#define ADC120_MIN_VALUE				(0)
#define ADC120_MAX_VALUE				(4095)

/** SPI Maximum Timeout values for flags waiting loops */
#define SPIx_TIMEOUT_MAX            ((uint32_t)0x1000)


/* Exported Types  -----------------------------------------------------------*/
typedef struct
{
  /**  Function pointer to Init */
  void (*Init)(void*);
  /** Function pointer to AttachErrorHandler */
  void (*AttachErrorHandler)(void (*callback)(uint16_t));
    /** Function pointer to ErrorHandler */
  void (*ErrorHandler)(uint16_t);
  /** Function pointer to ReadADCChannel */
  uint8_t (*ReadAdcChannel)(uint8_t);

} adc120Drv_t;

/* Exported functions --------------------------------------------------------*/
void ADC120_Init(void* pInit);                               	/* Start the ADC120 library */
void ADC120_AttachErrorHandler(void (*callback)(uint16_t));  	/* Attach a user callback to the error handler */
uint8_t Adc120_ReadAdcChannel(uint8_t);							/* Return analog value of a defined ADC channel */
adc120Drv_t* ADC120_GetAdc120Handle(void);		                /* Return handle of the ADC120 driver */

uint8_t ADC120_control_register_assembler(uint8_t adc_channel_id);
uint16_t ADC120_SPI_frame_assembler(uint8_t adc_control_register);
uint16_t ADC120_channel_read(SPI_HandleTypeDef *SpiHandle, uint8_t adc_channel_id);

#ifdef	 __cplusplus
}
#endif

#endif /* BSP_COMPONENTS_ADC120_INC_ADC120_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
