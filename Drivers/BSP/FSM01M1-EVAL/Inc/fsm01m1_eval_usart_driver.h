/*
 * STEVAL-FSM01M1-USART_driver.h
 *
 *  Created on: 23. 11. 2022
 *      Author: marek novotny
 */

#ifndef BSP_FSM01M1_EVAL_INC_FSM01M1_EVAL_USART_DRIVER_H_
#define BSP_FSM01M1_EVAL_INC_FSM01M1_EVAL_USART_DRIVER_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private define ------------------------------------------------------------*/
#define USART_MAX_MSG_LEN 1000

/* Exported types ------------------------------------------------------------*/
typedef enum USART_MsgFlag_t {
	idle,
	ready,
	wait,
	write,
	flush_write,
} USART_MsgFlagTypeDef;

typedef struct USART_Message_t {
	char data[USART_MAX_MSG_LEN];
	int length;
	USART_MsgFlagTypeDef flag;

	// aliases to common functions
	void (* AppendInt)(int i, struct USART_Message_t * msg);
	void (* AppendFloat)(float f, struct USART_Message_t * msg);
	void (* AppendStr)(char * str, struct USART_Message_t * msg);
	void (* Clear)(struct USART_Message_t * msg);
} USART_MessageTypeDef;

 /* Exported functions -------------------------------------------------------*/
// TODO: vystiznejsi nazvoslovi funkci
int FSM01M1_USART_vCOM_Config(UART_HandleTypeDef * huart);
USART_MessageTypeDef FSM01M1_USART_vCOM_CreateMessage();
void FSM01M1_USART_vCOM_AppendInt(int i, USART_MessageTypeDef * msg);
void FSM01M1_USART_vCOM_AppendFloat(float f, USART_MessageTypeDef * msg);
void FSM01M1_USART_vCOM_AppendDouble(double d, USART_MessageTypeDef * m);
void FSM01M1_USART_vCOM_AppendStr(char * str, USART_MessageTypeDef * msg);
void FSM01M1_USART_vCOM_Clear(USART_MessageTypeDef * msg);
HAL_StatusTypeDef FSM01M1_USART_vCOM_Write();
HAL_StatusTypeDef FSM01M1_USART_vCOM_FlushWrite(USART_MessageTypeDef * msg);
HAL_StatusTypeDef FSM01M1_USART_vCOM_FlushWriteLine(USART_MessageTypeDef * msg);
HAL_StatusTypeDef FSM01M1_USART_vCOM_WriteLine(USART_MessageTypeDef * msg);
HAL_StatusTypeDef FSM01M1_USART_vCOM_ReadLine(USART_MessageTypeDef * msg);
HAL_StatusTypeDef FSM01M1_USART_vCOM_WriteChar(char c);
HAL_StatusTypeDef FSM01M1_USART_vCOM_Status();


#endif /* BSP_FSM01M1_EVAL_INC_FSM01M1_EVAL_USART_DRIVER_H_ */
