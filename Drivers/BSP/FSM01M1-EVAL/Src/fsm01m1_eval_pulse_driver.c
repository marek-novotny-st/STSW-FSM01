/*
 * fsm01m1_eval_pulse_driver.c
 *
 *  Created on: Jan 18, 2023
 *      Author: marek novotny
 */

#include "fsm01m1_eval_pulse_driver.h"
#include "fsm01m1_eval_driver.h"
#include "main.h"

#define MICROS 1000000

void FSM01M1_PULSE_DSC_Reset(TIM_HandleTypeDef * htim);

/**
 * @brief Configures parameters of pulse generating timer
 * @param htim: configured PWM timer handle
 * @param TIM: timer struct
 * @param Channel: timer channel
 * @param Time_Base: duration of a tick in microseconds
 * @param Period_Ticks: duration of a period in ticks
 * @param Pulse_Ticks: duration of a pulse in ticks
 * @retval None
 */
void FSM01M1_PULSE_PulseGen_TIM_Config(TIM_HandleTypeDef * htim, TIM_TypeDef * TIM, uint32_t Channel, uint32_t Time_Base, uint32_t Period_Ticks, uint32_t Pulse_Ticks) {
	/* Clock section */
	uint32_t PCLK1Freq = HAL_RCC_GetPCLK1Freq();
	uint32_t PCLK2Freq = HAL_RCC_GetPCLK2Freq();
	if (PCLK1Freq != PCLK2Freq) Error_Handler();
	uint32_t BusFreq = PCLK1Freq;

	uint32_t Prescaler = (BusFreq/MICROS) * Time_Base;

	htim->Init.Prescaler = Prescaler;
	htim->Init.Period = Period_Ticks;
	if (HAL_TIM_PWM_Init(htim) != HAL_OK) Error_Handler();

	__HAL_LOCK(htim);
	switch (Channel) {
		case TIM_CHANNEL_1:
			TIM->CCR1 = Pulse_Ticks;
			break;
		case TIM_CHANNEL_2:
			TIM->CCR2 = Pulse_Ticks;
			break;
		case TIM_CHANNEL_3:
			TIM->CCR3 = Pulse_Ticks;
			break;
		case TIM_CHANNEL_4:
			TIM->CCR4 = Pulse_Ticks;
		default:
			Error_Handler();
			break;
	}
	__HAL_UNLOCK(htim);
}

/**
 * @brief starts pulse generating timer
 * @param htim: timer handle
 * @param channel: timer channel
 * @retval None
 */
void FSM01M1_PULSE_PulseGen_TIM_Start(TIM_HandleTypeDef * htim, uint32_t channel) {
	FSM01M1_PULSE_DSC_Reset(htim);
	HAL_TIM_PWM_Start(htim, channel);
}

/**
 * @brief stops pulse generating timer
 * @param htim: timer handle
 * @param channel: timer channel
 * @retval None
 */
void FSM01M1_PULSE_PulseGen_TIM_Stop(TIM_HandleTypeDef * htim, uint32_t channel) {
	HAL_TIM_PWM_Stop(htim, channel);
	FSM01M1_PULSE_DSC_Reset(htim);
}

/**
 * @brief starts pulse generating timer in interrupt mode
 * @param htim: timer handle
 * @param channel: timer channel
 * @retval None
 */
void FSM01M1_PULSE_PulseGen_TIM_Start_IT(TIM_HandleTypeDef * htim, uint32_t channel) {
	FSM01M1_PULSE_DSC_Reset(htim);
	HAL_TIM_Base_Start_IT(htim);
	HAL_TIM_PWM_Start_IT(htim, channel);
}

/**
 * @brief stops pulse generating timer in interrupt mode
 * @param htim: timer handle
 * @param channel: timer channel
 * @retval None
 */
void FSM01M1_PULSE_PulseGen_TIM_Stop_IT(TIM_HandleTypeDef * htim, uint32_t channel) {
	HAL_TIM_PWM_Stop_IT(htim, channel);
	HAL_TIM_Base_Stop_IT(htim);
	FSM01M1_PULSE_DSC_Reset(htim);
}

/**
 * @brief resets discharging circuits
 * @param htim: timer handle
 * @retval None
 */
void FSM01M1_PULSE_DSC_Reset(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM4) FSM01M1_OUT1_DSC_OFF();
	else if (htim->Instance == TIM1) FSM01M1_OUT2_DSC_OFF();
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	/* turn on discharge path as soon as pulse finishes */
	if (htim->Instance == TIM4) FSM01M1_OUT1_DSC_ON();
	else if (htim->Instance == TIM1) FSM01M1_OUT2_DSC_ON();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* turn off discharge path for new period */
	if (htim->Instance == TIM4) FSM01M1_OUT1_DSC_OFF();
	else if (htim->Instance == TIM1) FSM01M1_OUT2_DSC_OFF();
}
