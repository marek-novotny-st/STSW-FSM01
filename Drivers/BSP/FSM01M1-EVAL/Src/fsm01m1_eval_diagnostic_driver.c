/**
  ******************************************************************************
  * @file    fsm01m1_eval_diagnostic_driver.c
  * @author  ST Power Application Laboratory
  * @version V1.0.0
  * @brief   Provides functions for interactive board control and measurement
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "fsm01m1_eval_diagnostic_driver.h"
#include "fsm01m1_eval_driver.h"
#include "fsm01m1_eval_pulse_driver.h"
#include "fsm01m1_eval_exports.h"
#include "stdlib.h"
//#include "fsm01m1_eval_usart_driver.h"
#include "nucleo_usart_driver.h"
//#include "strtok_r.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {
	on,
	off,
	state,
	states,
	level,
	levels
} FSM01M1_DIAG_ActionTypeDef;

typedef enum USART_Format_t {
	numerical,
	logical
} USART_FormatTypeDef;

/* Private variables ---------------------------------------------------------*/

/* buffer for command tokenization */
static char * token_ctx;

/* registered devices */
static FSM01M1_DIAG_DeviceTypeDef devices[15] = {
		fsm01m1_vcc,
		fsm01m1_vcc1,
		fsm01m1_vcc1_dsc,
		fsm01m1_vcc2,
		fsm01m1_vcc2_dsc,
		fsm01m1_out1,
		fsm01m1_out1_dsc,
		fsm01m1_out2,
		fsm01m1_out2_dsc,
		fsm01m1_in1,
		fsm01m1_in2,
		fsm01m1_coff1,
		fsm01m1_coff2,
		fsm01m1_tp1,
		fsm01m1_tp2
};

static FSM01M1_DIAG_DeviceTypeDef outputs[2] = { fsm01m1_out1, fsm01m1_out2 };

static FSM01M1_DIAG_DeviceTypeDef sources[3] = { fsm01m1_vcc, fsm01m1_vcc1, fsm01m1_vcc2 };

/* registered actions */
static FSM01M1_DIAG_ActionTypeDef actions[6] = {
		on,
		off,
		state,
		states,
		level,
		levels
};

static USART_MessageTypeDef cmd;
static USART_MessageTypeDef msg;
/* Private function prototypes -----------------------------------------------*/
void FSM01M1_DIAG_splash_msg();
void FSM01M1_DIAG_read(FSM01M1_DIAG_DeviceTypeDef dev, USART_FormatTypeDef fmt);
void FSM01M1_DIAG_reset_devices();
void FSM01M1_DIAG_switch(FSM01M1_DIAG_DeviceTypeDef dev, FSM01M1_DIAG_ActionTypeDef act);
void FSM01M1_DIAG_resolve(char * cmd, FSM01M1_DIAG_DeviceTypeDef target);
void FSM01M1_DIAG_levels();
void FSM01M1_DIAG_states();
void FSM01M1_DIAG_help();
void FSM01M1_DIAG_list_devices();
void FSM01M1_DIAG_list_actions();
void FSM01M1_DIAG_pulse(DIAG_DeviceTypeDef dev, char * config_str);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Runs a diagnostic IO command line application
 * @param huart: uart handle
 * @retval None
 */
void FSM01M1_DIAG_IO_Loop(UART_HandleTypeDef * huart) {
	NUCLEO_USART_vCOM_Config(huart);
	cmd = NUCLEO_USART_vCOM_CreateMessage();
	msg = NUCLEO_USART_vCOM_CreateMessage();

	FSM01M1_DIAG_splash_msg();
	while(1) {
		if (cmd.flag == ready) {
			FSM01M1_DIAG_resolve(cmd.data, fsm01m1_all);
			cmd.Reset(&cmd);
			cmd.flag = idle;
		}
		if (cmd.flag == idle) {
			NUCLEO_USART_vCOM_ReadLine(&cmd);
		}
	}
}

/**
 * @brief Provides command resolution services
 * @param cmd: command
 * @param target: device
 * @retval None
 */
void FSM01M1_DIAG_resolve(char * cmd, FSM01M1_DIAG_DeviceTypeDef target) {
	if (cmd[0] == '\r' || cmd[0] == '\n') cmd = cmd + 1;
	char * arg = (char *) strtok_r(cmd, " ", &token_ctx);

	if (arg == NULL) return;
	arg[strcspn(arg, "\r\n")] = '\0';

	if (arg[0] == '\0') return;
	else if (strcmp(arg, "vcc") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_vcc);
	else if (strcmp(arg, "vcc1") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_vcc1);
	else if (strcmp(arg, "vcc1_dsc") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_vcc1_dsc);
	else if (strcmp(arg, "vcc2") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_vcc2);
	else if (strcmp(arg, "vcc2_dsc") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_vcc2_dsc);
	else if (strcmp(arg, "vccx") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_vccx);
	else if (strcmp(arg, "out1") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_out1);
	else if (strcmp(arg, "out1_dsc") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_out1_dsc);
	else if (strcmp(arg, "out2") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_out2);
	else if (strcmp(arg, "out2_dsc") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_out2_dsc);
	else if (strcmp(arg, "outx") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_outx);
	else if (strcmp(arg, "coff1") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_coff1);
	else if (strcmp(arg, "coff2") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_coff2);
	else if (strcmp(arg, "tp1") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_tp1);
	else if (strcmp(arg, "tp2") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_tp2);
	else if (strcmp(arg, "in1") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_in1);
	else if (strcmp(arg, "in2") == 0) FSM01M1_DIAG_resolve(NULL, fsm01m1_in2);
	else if (strcmp(arg, "on") == 0) FSM01M1_DIAG_switch(target, on);
	else if (strcmp(arg, "off") == 0) FSM01M1_DIAG_switch(target, off);
	else if (strcmp(arg, "state") == 0) FSM01M1_DIAG_read(target, logical);
	else if (strcmp(arg, "level") == 0) FSM01M1_DIAG_read(target, numerical);
	else if (strcmp(arg, "levels") == 0) FSM01M1_DIAG_levels();
	else if (strcmp(arg, "states") == 0) FSM01M1_DIAG_states();
	else if (strcmp(arg, "help") == 0) FSM01M1_DIAG_help();
	else if (strcmp(arg, "functions") == 0) FSM01M1_DIAG_list_devices();
	else if (strcmp(arg, "actions") == 0) FSM01M1_DIAG_list_actions();
	else if (strcmp(arg, "clear") == 0) NUCLEO_USART_vCOM_Clear();
	else {
		msg.Reset(&msg);
		msg.AppendStr("Invalid command, no actions performed", &msg);
		NUCLEO_USART_vCOM_WriteLine(&msg);
	}
}

/**
 * @brief Initializes command data structure
 * @param constant pointer to command data structure
 * @retval None
 */
void FSM01M1_DIAG_InitCmd(const USART_MessageTypeDef* p_cmd) {
	cmd = *p_cmd;
}

/**
 * @brief Initializes message data structure
 * @param constant pointer to message data structure
 * @retval None
 */
void FSM01M1_DIAG_InitMsg(const USART_MessageTypeDef* p_msg) {
	msg = *p_msg;

}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Prints starting message
 * @retval None
 */
void FSM01M1_DIAG_splash_msg() {
	msg.Reset(&msg);
	msg.AppendStr("***** STEVAL-FSM01M1 DIAGNOSTIC TOOL *****\n", &msg);
	msg.AppendStr("* Type help for usage information", &msg);
	NUCLEO_USART_vCOM_WriteLine(&msg);
	NUCLEO_USART_vCOM_WriteChar('\n');
}

/**
 * @brief Prints help message
 * @retval None
 */
void FSM01M1_DIAG_help() {
	msg.Reset(&msg);
	msg.AppendStr("[function] [action] - applies action to a function\n"
			"[global_action] - applies action to all functions\n"
			"- Type 'functions' for function list\n"
			"- Type 'actions' for action list\n"
			"- Type 'clear' to clear text from terminal\n"
			/*"- Use 'x' in device identifiers for numerical wildcard (e.g. outx selects all outputs)\n"*/, &msg);
	NUCLEO_USART_vCOM_Write(&msg);
}

/**
 * @brief Prints list of devices
 * @retval None
 */
void FSM01M1_DIAG_list_devices() {
	int dev_count = sizeof(devices)/sizeof(FSM01M1_DIAG_DeviceTypeDef);

	msg.Reset(&msg);
	for (int i = 0; i < dev_count; i += 1) {
		switch (devices[i]) {
			case fsm01m1_vcc:
				msg.AppendStr("vcc\n", &msg);
				break;
			case fsm01m1_vcc1:
				msg.AppendStr("vcc1\n", &msg);
				break;
			case fsm01m1_vcc1_dsc:
				msg.AppendStr("vcc1_dsc\n", &msg);
				break;
			case fsm01m1_vcc2:
				msg.AppendStr("vcc2\n", &msg);
				break;
			case fsm01m1_vcc2_dsc:
				msg.AppendStr("vcc2_dsc\n", &msg);
				break;
			case fsm01m1_in1:
				msg.AppendStr("in1\n", &msg);
				break;
			case fsm01m1_in2:
				msg.AppendStr("in2\n", &msg);
				break;
			case fsm01m1_out1:
				msg.AppendStr("out1\n", &msg);
				break;
			case fsm01m1_out1_dsc:
				msg.AppendStr("out1_dsc\n", &msg);
				break;
			case fsm01m1_out2:
				msg.AppendStr("out2\n", &msg);
				break;
			case fsm01m1_out2_dsc:
				msg.AppendStr("out2_dsc\n", &msg);
				break;
			case fsm01m1_coff1:
				msg.AppendStr("coff1\n", &msg);
				break;
			case fsm01m1_coff2:
				msg.AppendStr("coff2\n", &msg);
				break;
			case fsm01m1_tp1:
				msg.AppendStr("tp1\n", &msg);
				break;
			case fsm01m1_tp2:
				msg.AppendStr("tp2\n", &msg);
				break;
			default:
				break;
		}
	}

	NUCLEO_USART_vCOM_Write(&msg);
}

/**
 * @brief Prints list of actions
 * @retval None
 */
void FSM01M1_DIAG_list_actions() {
	int act_len = sizeof(actions)/sizeof(FSM01M1_DIAG_ActionTypeDef);

	msg.Reset(&msg);
	for (int i = 0; i < act_len; i += 1) {
		switch (actions[i]) {
			case on:
				msg.AppendStr("on\n", &msg);
				break;
			case off:
				msg.AppendStr("off\n", &msg);
				msg.AppendStr("off [global]\n", &msg);
				break;
			case state:
				msg.AppendStr("state - returns if function is on or off\n", &msg);
				break;
			case states:
				msg.AppendStr("states [global]\n", &msg);
				break;
			case level:
				msg.AppendStr("level - returns numeric representation of function state"
						"(integer 1 and 0 denote logical state, decimals denote physical state)\n", &msg);
				break;
			case levels:
				msg.AppendStr("levels [global]\n", &msg);
				break;
			default:
				break;
		}
	}

	FSM01M1_USART_vCOM_Write(&msg);
}

/**
 * @brief Provides command resolution services
 * @param cmd: command
 * @param target: device
 * @retval None
 */
void FSM01M1_DIAG_resolve(char * cmd, DIAG_DeviceTypeDef target) {
	if (cmd[0] == '\r' || cmd[0] == '\n') cmd = cmd + 1;
	char * arg = (char *) strtok_r(cmd, " ", &token_ctx);

	if (arg == NULL) return;
	arg[strcspn(arg, "\r\n")] = '\0';

	if (arg[0] == '\0') return;
	else if (strcmp(arg, "vcc") == 0) FSM01M1_DIAG_resolve(NULL, vcc);
	else if (strcmp(arg, "vcc1") == 0) FSM01M1_DIAG_resolve(NULL, vcc1);
	else if (strcmp(arg, "vcc1_dsc") == 0) FSM01M1_DIAG_resolve(NULL, vcc1_dsc);
	else if (strcmp(arg, "vcc2") == 0) FSM01M1_DIAG_resolve(NULL, vcc2);
	else if (strcmp(arg, "vcc2_dsc") == 0) FSM01M1_DIAG_resolve(NULL, vcc2_dsc);
	else if (strcmp(arg, "vccx") == 0) FSM01M1_DIAG_resolve(NULL, vccx);
	else if (strcmp(arg, "out1") == 0) FSM01M1_DIAG_resolve(NULL, out1);
	else if (strcmp(arg, "out1_dsc") == 0) FSM01M1_DIAG_resolve(NULL, out1_dsc);
	else if (strcmp(arg, "out2") == 0) FSM01M1_DIAG_resolve(NULL, out2);
	else if (strcmp(arg, "out2_dsc") == 0) FSM01M1_DIAG_resolve(NULL, out2_dsc);
	else if (strcmp(arg, "outx") == 0) FSM01M1_DIAG_resolve(NULL, outx);
	else if (strcmp(arg, "coff1") == 0) FSM01M1_DIAG_resolve(NULL, coff1);
	else if (strcmp(arg, "coff2") == 0) FSM01M1_DIAG_resolve(NULL, coff2);
	else if (strcmp(arg, "tp1") == 0) FSM01M1_DIAG_resolve(NULL, tp1);
	else if (strcmp(arg, "tp2") == 0) FSM01M1_DIAG_resolve(NULL, tp2);
	else if (strcmp(arg, "in1") == 0) FSM01M1_DIAG_resolve(NULL, in1);
	else if (strcmp(arg, "in2") == 0) FSM01M1_DIAG_resolve(NULL, in2);
	else if (strcmp(arg, "on") == 0) FSM01M1_DIAG_switch(target, on);
	else if (strcmp(arg, "off") == 0) FSM01M1_DIAG_switch(target, off);
	else if (strcmp(arg, "state") == 0) FSM01M1_DIAG_read(target, logical);
	else if (strcmp(arg, "level") == 0) FSM01M1_DIAG_read(target, numerical);
	else if (strcmp(arg, "levels") == 0) FSM01M1_DIAG_levels();
	else if (strcmp(arg, "states") == 0) FSM01M1_DIAG_states();
	else if (strcmp(arg, "help") == 0) FSM01M1_DIAG_help();
	else if (strcmp(arg, "functions") == 0) FSM01M1_DIAG_list_devices();
	else if (strcmp(arg, "actions") == 0) FSM01M1_DIAG_list_actions();
	else if (strcmp(arg, "clear") == 0) FSM01M1_USART_vCOM_Clear();
	else if (strcmp(arg, "pulse") == 0) FSM01M1_DIAG_pulse(target, cmd);
	else {
		msg.Reset(&msg);
		msg.AppendStr("Invalid command, no actions performed", &msg);
		FSM01M1_USART_vCOM_WriteLine(&msg);
	}
}

/**
 * @brief Provides device switching services
 * @param dev: device
 * @param act: action
 * @retval None
 */
void FSM01M1_DIAG_switch(FSM01M1_DIAG_DeviceTypeDef dev, FSM01M1_DIAG_ActionTypeDef act) {
	switch (dev) {
		case fsm01m1_vcc1:
			if (act == on) FSM01M1_VCC1_ON();
			else FSM01M1_VCC1_OFF();
			break;
		case fsm01m1_vcc1_dsc:
			if (act == on) FSM01M1_VCC1_DSC_ON();
			else FSM01M1_VCC1_DSC_OFF();
			break;
		case fsm01m1_vcc2:
			if (act == on) FSM01M1_VCC2_ON();
			else FSM01M1_VCC2_OFF();
			break;
		case fsm01m1_vcc2_dsc:
			if (act == on) FSM01M1_VCC2_DSC_ON();
			else FSM01M1_VCC2_DSC_OFF();
			break;
		case fsm01m1_vccx: {
				int vcc_count = sizeof(sources)/sizeof(FSM01M1_DIAG_DeviceTypeDef);
				for (int i = 0; i < vcc_count; i += 1) {
					FSM01M1_DIAG_switch(sources[i], act);
				}
			}
			break;
		case fsm01m1_out1:
			if (act == on) FSM01M1_OUT1_CTRL_ON();
			else FSM01M1_OUT1_CTRL_OFF();
			break;
		case fsm01m1_out1_dsc:
			if (act == on) FSM01M1_OUT1_DSC_ON();
			else FSM01M1_OUT1_DSC_OFF();
			break;
		case fsm01m1_out2:
			if (act == on) FSM01M1_OUT2_CTRL_ON();
			else FSM01M1_OUT2_CTRL_OFF();
			break;
		case fsm01m1_out2_dsc:
			if (act == on) FSM01M1_OUT2_DSC_ON();
			else FSM01M1_OUT2_DSC_OFF();
			break;

		case fsm01m1_outx: {
				int out_count = sizeof(outputs)/sizeof(FSM01M1_DIAG_DeviceTypeDef);
				for (int i = 0; i < out_count; i += 1) {
					FSM01M1_DIAG_switch(outputs[i], act);
				}
			}
			break;
		case fsm01m1_coff1:
			if (act == on) FSM01M1_CUTOFF1_CTRL_ON();
			else FSM01M1_CUTOFF1_CTRL_OFF();
			break;
		case fsm01m1_coff2:
			if (act == on) FSM01M1_CUTOFF2_CTRL_ON();
			else FSM01M1_CUTOFF2_CTRL_OFF();
			break;
		case fsm01m1_tp1:
			if (act == on) FSM01M1_TP1_ON();
			else FSM01M1_TP1_OFF();
			break;
		case fsm01m1_tp2:
			if (act == on) FSM01M1_TP2_ON();
			else FSM01M1_TP2_OFF();
			break;
		case fsm01m1_all:
			if (act == off) FSM01M1_system_deactivation();
		default:
			break;
	}
}

/**
 * @brief Provides device reading services
 * @param dev: device
 * @param fmt: reading formatting
 * @retval None
 */
void FSM01M1_DIAG_read(FSM01M1_DIAG_DeviceTypeDef dev, USART_FormatTypeDef fmt) {
	msg.Reset(&msg);

	int logic = -1;
	float reading = -1.0;

	switch (dev) {
		case fsm01m1_vcc:
			reading = FSM01M1_ADC120_read_single_node(&hspi2, VCC_ADC_CHANNEL_ID);
			msg.AppendStr("VCC \t\t = ", &msg);
			break;
		case fsm01m1_vcc1:
			reading = FSM01M1_ADC120_read_single_node(&hspi2, VCC1_ADC_CHANNEL_ID);
			msg.AppendStr("VCC1 \t\t = ", &msg);
			break;
		case fsm01m1_vcc1_dsc:
			logic = (int) HAL_GPIO_ReadPin(VCC1_DSC_GPIO_Port, VCC1_DSC_Pin);
			msg.AppendStr("VCC1_DSC \t = ", &msg);
			break;
		case fsm01m1_vcc2:
			reading = FSM01M1_ADC120_read_single_node(&hspi2, VCC2_ADC_CHANNEL_ID);
			msg.AppendStr("VCC2 \t\t = ", &msg);
			break;
		case fsm01m1_vcc2_dsc:
			logic = (int) HAL_GPIO_ReadPin(VCC2_DSC_GPIO_Port, VCC2_DSC_Pin);
			msg.AppendStr("VCC2_DSC \t = ", &msg);
			break;
		case fsm01m1_in1:
			logic = (int) HAL_GPIO_ReadPin(OUTP1_GPIO_Port, OUTP1_Pin);
			msg.AppendStr("IN1 \t\t = ", &msg);
			break;
		case fsm01m1_in2:
			logic = (int) HAL_GPIO_ReadPin(OUTP2_GPIO_Port, OUTP2_Pin);
			msg.AppendStr("IN2 \t\t = ", &msg);
			break;
		case fsm01m1_out1:
			reading = FSM01M1_ADC120_read_single_node(&hspi2, OUT1_ADC_CHANNEL_ID);
			msg.AppendStr("OUT1 \t\t = ", &msg);
			break;
		case fsm01m1_out1_dsc:
			logic = (int) HAL_GPIO_ReadPin(OUT1_DSC_GPIO_Port, OUT1_DSC_Pin);
			msg.AppendStr("OUT1_DSC \t = ", &msg);
			break;
		case fsm01m1_out2:
			reading = FSM01M1_ADC120_read_single_node(&hspi2, OUT2_ADC_CHANNEL_ID);
			msg.AppendStr("OUT2 \t\t = ", &msg);
			break;
		case fsm01m1_out2_dsc:
			logic = (int) HAL_GPIO_ReadPin(OUT2_DSC_GPIO_Port, OUT2_DSC_Pin);
			msg.AppendStr("OUT2_DSC \t = ", &msg);
			break;
		case fsm01m1_coff1:
			logic = (int) HAL_GPIO_ReadPin(COFF1_CTRL_GPIO_Port, COFF1_CTRL_Pin) == GPIO_PIN_RESET;
			msg.AppendStr("COFF1 \t\t = ", &msg);
			break;
		case fsm01m1_coff2:
			logic = (int) HAL_GPIO_ReadPin(COFF2_CTRL_GPIO_Port, COFF2_CTRL_Pin) == GPIO_PIN_RESET;
			msg.AppendStr("COFF2 \t\t = ", &msg);
			break;
		case fsm01m1_tp1:
			logic = (int) HAL_GPIO_ReadPin(TP1_CTRL_GPIO_Port, TP1_CTRL_Pin) == GPIO_PIN_RESET;
			msg.AppendStr("TP1 \t\t = ", &msg);
			break;
		case fsm01m1_tp2:
			logic = (int) HAL_GPIO_ReadPin(TP2_CTRL_GPIO_Port, TP2_CTRL_Pin) == GPIO_PIN_RESET;
			msg.AppendStr("TP2 \t\t = ", &msg);
			break;
		default:
			break;
	}

	if (fmt == numerical) {
		if (reading != -1.0f) msg.AppendFloat(reading, &msg);
		else if (logic != -1) msg.AppendInt(logic, &msg);
	}
	else if (fmt == logical) {
		if (reading >= FSM01M1_NOMINAL_VOLTAGE_THRESHOLD || logic == 1) msg.AppendStr("(on)", &msg);
		else if (reading <= FSM01M1_ZERO_VOLTAGE_THRESHOLD || logic == 0) msg.AppendStr("(off)", &msg);
		else msg.AppendStr("(?)", &msg);
	}

	msg.AppendStr("\n", &msg);
	NUCLEO_USART_vCOM_Write(&msg);
}

/**
 * @brief Prints voltage and logical levels of all devices
 * @retval None
 */
void FSM01M1_DIAG_levels() {
	int dev_count = sizeof(devices)/sizeof(FSM01M1_DIAG_DeviceTypeDef);

	for(int i = 0; i < dev_count; i += 1)
		FSM01M1_DIAG_read(devices[i], numerical);
}

/**
 * @brief Prints switch states of all devices
 * @note '?' output symbol means switch state can not be determined
 * @retval None
 */
void FSM01M1_DIAG_states() {
	int dev_count = sizeof(devices)/sizeof(FSM01M1_DIAG_DeviceTypeDef);

	for(int i = 0; i < dev_count; i += 1)
		FSM01M1_DIAG_read(devices[i], logical);
}

void FSM01M1_DIAG_pulse(DIAG_DeviceTypeDef dev, char * config_str) {
	char * pulse_params[2] = {0};
	for (unsigned int i = 0; i < 2; i += 1) {
		char ** param_ptr = &pulse_params[i];
		*param_ptr = (char *) strtok_r(config_str, ",", &token_ctx);
		uint32_t len = strcspn(*param_ptr, "\r\n");
		*param_ptr[len] = '\0';
	}

	switch (dev) {
		case out1:
			FSM01M1_PULSE_PulseGen_TIM_IT(OUT1_TIM_HANDLE, OUT1_TIM, OUT1_TIM_CHANNEL, 1, 65535, 25000);
			break;
		case out2:
			FSM01M1_PULSE_PulseGen_TIM_IT(OUT2_TIM_HANDLE, OUT2_TIM, OUT2_TIM_CHANNEL, 1, 65535, 25000);
			break;
		default:
			// print informative message
			break;
	}
}
