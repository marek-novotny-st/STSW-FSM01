/*
 * fsm01m1_eval_pulse_driver.h
 *
 *  Created on: Jan 18, 2023
 *      Author: marek novotny
 */

#ifndef BSP_FSM01M1_EVAL_INC_FSM01M1_EVAL_PULSE_DRIVER_H_
#define BSP_FSM01M1_EVAL_INC_FSM01M1_EVAL_PULSE_DRIVER_H_

#include "stm32f4xx_hal.h"

/* FSM01M1 specific timers */
#define OUT1_TIM TIM4
#define OUT2_TIM TIM1

/* FSM01M1 specific channels */
#define OUT1_TIM_CHANNEL TIM_CHANNEL_3
#define OUT2_TIM_CHANNEL TIM_CHANNEL_4


HAL_StatusTypeDef FSM01M1_PULSE_PulseGen_TIM_Config(TIM_HandleTypeDef * htim, TIM_TypeDef * TIM, uint32_t Channel, uint32_t Time_Base, uint32_t Period_Ticks, uint32_t Pulse_Ticks);
void FSM01M1_PULSE_PulseGen_TIM_Start(TIM_HandleTypeDef * htim, uint32_t channel);
void FSM01M1_PULSE_PulseGen_TIM_Stop(TIM_HandleTypeDef *htim, uint32_t channel);
void FSM01M1_PULSE_PulseGen_TIM_Start_IT(TIM_HandleTypeDef * htim, uint32_t channel);
void FSM01M1_PULSE_PulseGen_TIM_Stop_IT(TIM_HandleTypeDef *htim, uint32_t channel);

#endif /* BSP_FSM01M1_EVAL_INC_FSM01M1_EVAL_PULSE_DRIVER_H_ */
