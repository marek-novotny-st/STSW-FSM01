/*
 * fsm01m1_eval_pulse_driver.c
 *
 *  Created on: Jan 18, 2023
 *      Author: marek novotny
 */

#include "fsm01m1_eval_pulse_driver.h"
//#include "stm32f4xx_hal.h"

#define MICRO_FREQ 1000000

/**
 * @brief Creates instance of timer for generating pulsed signals
 * @param TIM: timer struct
 * @param Time_Base: duration of a tick in microseconds
 * @param Period_Time: duration of a period in ticks
 * @param Pulse_Time: duration of a pulse in ticks
 * @param htim_out: reference to output timer handle
 * @retval None
 */
void FSM01M1_PULSE_PulseGen_TIM_Init(TIM_TypeDef * TIM, uint32_t Time_Base, uint32_t Period_Ticks, uint32_t Pulse_Ticks, uint32_t Channel, TIM_HandleTypeDef * htim_out) {
	/* Clock section */
	uint32_t PCLK1Freq = HAL_RCC_GetPCLK1Freq();
	uint32_t PCLK2Freq = HAL_RCC_GetPCLK2Freq();
	if (PCLK1Freq != PCLK2Freq) Error_Handler();
	uint32_t BusFreq = PCLK1Freq;

	uint32_t Prescaler = (BusFreq/MICRO_FREQ) * Time_Base;

	/* Config section */
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim_out->Instance = TIM;
	htim_out->Init.Prescaler = Prescaler;
	htim_out->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim_out->Init.Period = 65535;
	htim_out->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim_out->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&htim_out) != HAL_OK)
	{
	Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim_out, &sMasterConfig) != HAL_OK)
	{
	Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 25000;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim_out, &sConfigOC, Channel) != HAL_OK)
	{
	Error_Handler();
	}

	HAL_TIM_MspPostInit(&htim_out);
}

void FSM01M1_PULSE_PulseGen_TIM_StartPWM(TIM_HandleTypeDef * htim, uint32_t channel) {
	HAL_TIM_PWM_Start(htim, channel);
}
