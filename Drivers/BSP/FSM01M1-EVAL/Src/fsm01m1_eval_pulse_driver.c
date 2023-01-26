/*
 * fsm01m1_eval_pulse_driver.c
 *
 *  Created on: Jan 18, 2023
 *      Author: marek novotny
 */

#include "fsm01m1_eval_pulse_driver.h"
#include "main.h"

#define MICROS 1000000

/**
 * @brief Creates instance of timer for generating pulsed signals
 * @param TIM: timer struct
 * @param Channel: timer channel
 * @param Time_Base: duration of a tick in microseconds
 * @param Period_Ticks: duration of a period in ticks
 * @param Pulse_Ticks: duration of a pulse in ticks
 * @param Export_Handle: reference to exporting timer handle
 * @retval None
 */
void FSM01M1_PULSE_PulseGen_TIM_Init(TIM_TypeDef * TIM, uint32_t Channel, uint32_t Time_Base, uint32_t Period_Ticks, uint32_t Pulse_Ticks, TIM_HandleTypeDef * Export_Handle) {
	/* Clock section */
	uint32_t PCLK1Freq = HAL_RCC_GetPCLK1Freq();
	uint32_t PCLK2Freq = HAL_RCC_GetPCLK2Freq();
	if (PCLK1Freq != PCLK2Freq) Error_Handler();
	uint32_t BusFreq = PCLK1Freq;

	uint32_t Prescaler = (BusFreq/MICROS) * Time_Base;

	/* Config section */
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	Export_Handle->Instance = TIM;
	Export_Handle->Init.Prescaler = Prescaler;
	Export_Handle->Init.CounterMode = TIM_COUNTERMODE_UP;
	Export_Handle->Init.Period = Period_Ticks;
	Export_Handle->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	Export_Handle->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(Export_Handle) != HAL_OK)
	{
	Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(Export_Handle, &sMasterConfig) != HAL_OK)
	{
	Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = Pulse_Ticks;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(Export_Handle, &sConfigOC, Channel) != HAL_OK)
	{
	Error_Handler();
	}

	HAL_TIM_MspPostInit(Export_Handle);
}

/**
 * @brief starts pulse generating timer
 * @param htim: timer handle
 * @param channel: timer channel
 * @retval None
 */
void FSM01M1_PULSE_PulseGen_TIM_Start(TIM_HandleTypeDef * htim, uint32_t channel) {
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
}

