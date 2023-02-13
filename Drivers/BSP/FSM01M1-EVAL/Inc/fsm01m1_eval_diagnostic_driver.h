/*
 * STEVAL-FSM01M1-diagnostic_driver.h
 *
 *  Created on: 28. 11. 2022
 *      Author: marek novotny
 */

#ifndef BSP_FSM01M1_EVAL_INC_FSM01M1_EVAL_DIAGNOSTIC_DRIVER_H_
#define BSP_FSM01M1_EVAL_INC_FSM01M1_EVAL_DIAGNOSTIC_DRIVER_H_
/* Includes ------------------------------------------------------------------*/
#include <fsm01m1_eval_usart_driver.h>
#include "stdbool.h"


/* Exported types -------------------------------------------------------*/
typedef enum {
	all,
	vcc,
	vcc1,
	vcc1_dsc,
	vcc2,
	vcc2_dsc,
	vccx,
	out1,
	out1_dsc,
	out2,
	out2_dsc,
	outx,
	in1,
	in2,
	coff1,
	coff2,
	tp1,
	tp2
} DIAG_DeviceTypeDef;

typedef enum {
	on,
	off,
	read,
	state,
	states,
	level,
	levels
} DIAG_ActionTypeDef;

/* Exported functions -------------------------------------------------------*/
void FSM01M1_DIAG_IO_Loop(UART_HandleTypeDef * huart);

#endif /* BSP_FSM01M1_EVAL_INC_FSM01M1_EVAL_DIAGNOSTIC_DRIVER_H_ */
