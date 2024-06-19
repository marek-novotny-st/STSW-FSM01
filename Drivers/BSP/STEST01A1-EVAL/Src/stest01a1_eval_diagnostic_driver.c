/**
  ******************************************************************************
  * @file    stest01a1_eval_diagnostic_driver.c
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
#include "stest01a1_eval_diagnostic_driver.h"
#include "stest01a1_eval_driver.h"
#include "nucleo_usart_driver.h"
//#include "strtok_r.h"
#include "string.h"
#include <math.h>

/* Private typedef -----------------------------------------------------------*/
typedef enum {
	on,
	off,
	state,
	states,
	level,
	levels
} STEST01A1_DIAG_ActionTypeDef;


typedef enum USART_Format_t {
	numerical,
	logical
} USART_FormatTypeDef;

/* Private variables ---------------------------------------------------------*/

/* buffer for command tokenization */
static char * token_ctx;

/* registered devices */
static STEST01A1_DIAG_DeviceTypeDef devices[1] = {
		stest01a1_out
};

static STEST01A1_DIAG_DeviceTypeDef outputs[1] = { stest01a1_out };

//STEST01A1_DIAG_DeviceTypeDef sources[3] = { vcc };

/* registered actions */
static STEST01A1_DIAG_ActionTypeDef actions[6] = {
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
void STEST01A1_DIAG_splash_msg();
void STEST01A1_DIAG_read(STEST01A1_DIAG_DeviceTypeDef dev, USART_FormatTypeDef fmt);
void STEST01A1_DIAG_reset_devices();
void STEST01A1_DIAG_switch(STEST01A1_DIAG_DeviceTypeDef dev, STEST01A1_DIAG_ActionTypeDef act);
void STEST01A1_DIAG_resolve(char * cmd, STEST01A1_DIAG_DeviceTypeDef target);
void STEST01A1_DIAG_levels();
void STEST01A1_DIAG_states();
void STEST01A1_DIAG_help();
void STEST01A1_DIAG_list_devices();
void STEST01A1_DIAG_list_actions();
void STEST01A1_DIAG_single_pulse(STEST01A1_DIAG_DeviceTypeDef dev, uint32_t duration);
void STEST01A1_DIAG_demag_stat();

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Runs a diagnostic IO command line application
 * @param huart: uart handle
 * @retval None
 */
void STEST01A1_DIAG_IO_Loop(UART_HandleTypeDef * huart) {
	NUCLEO_USART_vCOM_Config(huart);
	cmd = NUCLEO_USART_vCOM_CreateMessage();
	msg = NUCLEO_USART_vCOM_CreateMessage();

	STEST01A1_DIAG_splash_msg();
	while(1) {
		if (cmd.flag == ready) {
			STEST01A1_DIAG_resolve(cmd.data, stest01a1_all);
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
void STEST01A1_DIAG_resolve(char * cmd, STEST01A1_DIAG_DeviceTypeDef target) {
	if (cmd[0] == '\r' || cmd[0] == '\n') cmd = cmd + 1;
	char * arg = (char *) strtok_r(cmd, " ", &token_ctx);

	if (arg == NULL) return;
	arg[strcspn(arg, "\r\n")] = '\0';

	if (arg[0] == '\0') return;
	else if (strcmp(arg, "out") == 0) STEST01A1_DIAG_resolve(NULL, stest01a1_out);
	else if (strcmp(arg, "flt1") == 0) STEST01A1_DIAG_resolve(NULL, stest01a1_flt1);
	else if (strcmp(arg, "flt2") == 0) STEST01A1_DIAG_resolve(NULL, stest01a1_flt2);
	else if (strcmp(arg, "demag_stat") == 0) STEST01A1_DIAG_demag_stat();
	else if (strcmp(arg, "on") == 0) STEST01A1_DIAG_switch(target, on);
	else if (strcmp(arg, "off") == 0) STEST01A1_DIAG_switch(target, off);
	else if (strcmp(arg, "single") == 0) STEST01A1_DIAG_single_pulse(target, 1000);
	else if (strcmp(arg, "state") == 0) STEST01A1_DIAG_read(target, logical);
	else if (strcmp(arg, "level") == 0) STEST01A1_DIAG_read(target, numerical);
	else if (strcmp(arg, "levels") == 0) STEST01A1_DIAG_levels();
	else if (strcmp(arg, "states") == 0) STEST01A1_DIAG_states();
	else if (strcmp(arg, "help") == 0) STEST01A1_DIAG_help();
	else if (strcmp(arg, "functions") == 0) STEST01A1_DIAG_list_devices();
	else if (strcmp(arg, "actions") == 0) STEST01A1_DIAG_list_actions();
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
 * @retval const USART_MessageTypeDef*
 */
void STEST01A1_DIAG_InitCmd(const USART_MessageTypeDef* p_cmd) {
	cmd = *p_cmd;
}

/**
 * @brief Initializes message data structure
 * @param constant pointer to message structure
 * @retval const USART_MessageTypeDef*
 */
void STEST01A1_DIAG_InitMsg(const USART_MessageTypeDef* p_msg) {
	msg = *p_msg;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Prints starting message
 * @retval None
 */
void STEST01A1_DIAG_splash_msg() {
	msg.Reset(&msg);
	msg.AppendStr("***** STEST-01A1 DIAGNOSTIC TOOL *****\n", &msg);
	msg.AppendStr("* Type help for usage information", &msg);
	NUCLEO_USART_vCOM_WriteLine(&msg);
	NUCLEO_USART_vCOM_WriteChar('\n');
}

/**
 * @brief Prints help message
 * @retval None
 */
void STEST01A1_DIAG_help() {
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
void STEST01A1_DIAG_list_devices() {

	msg.Reset(&msg);
	for (int i = stest01a1_out; i <= stest01a1_flt2; i += 1) {
		switch (i) {
			case stest01a1_out:
				msg.AppendStr("out\n", &msg);
				break;
			case stest01a1_flt1:
				msg.AppendStr("flt1\n", &msg);
				break;
			case stest01a1_flt2:
				msg.AppendStr("flt2\n", &msg);
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
void STEST01A1_DIAG_list_actions() {

	msg.Reset(&msg);
	for (int i = on; i <= levels; i += 1) {
		switch (i) {
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

	NUCLEO_USART_vCOM_Write(&msg);
}

/**
 * @brief Provides device switching services
 * @param dev: device
 * @param act: action
 * @retval None
 */
void STEST01A1_DIAG_switch(STEST01A1_DIAG_DeviceTypeDef dev, STEST01A1_DIAG_ActionTypeDef act) {
	switch (dev) {
		case stest01a1_out:
			if (act == on) STEST01A1_OUT_ON();
			else STEST01A1_OUT_OFF();
			break;
		case stest01a1_all:
			if (act == off) STEST01A1_OUT_OFF();
		default:
			break;
	}
}

/**
 * @brief Provides device single pulse services
 * @param dev: device
 * @param act: action
 * @retval None
 */
void STEST01A1_DIAG_single_pulse(STEST01A1_DIAG_DeviceTypeDef dev, uint32_t duration) {
	switch (dev) {
		case stest01a1_out:
			STEST01A1_OUT_ON();
			HAL_Delay(duration);
			STEST01A1_OUT_OFF();
			break;
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
void STEST01A1_DIAG_read(STEST01A1_DIAG_DeviceTypeDef dev, USART_FormatTypeDef fmt) {
	msg.Reset(&msg);

	int logic = -1;
	float reading = -1.0;

	switch (dev) {
		case stest01a1_out:
			logic = (int) HAL_GPIO_ReadPin(OUT_CTRL_GPIO_Port, OUT_CTRL_Pin);
			msg.AppendStr("OUT \t\t = ", &msg);
			break;
		case stest01a1_flt1:
			logic = (int) STEST01A1_FLT1;
			STEST01A1_FLT1 = 0;
			msg.AppendStr("FLT1 \t\t = ", &msg);
			break;
		case stest01a1_flt2:
			logic = (int) STEST01A1_FLT2;
			STEST01A1_FLT2 = 0;
			msg.AppendStr("FLT2 \t\t = ", &msg);
			break;
		default:
			break;
	}

	if (fmt == numerical) {
		if (reading != -1.0f) msg.AppendFloat(reading, &msg);
		else if (logic != -1) msg.AppendInt(logic, &msg);
	}
	else if (fmt == logical) {
		if (reading >= STEST01A1_NOMINAL_VOLTAGE_THRESHOLD || logic == 1) msg.AppendStr("(on)", &msg);
		else if (reading <= STEST01A1_ZERO_VOLTAGE_THRESHOLD || logic == 0) msg.AppendStr("(off)", &msg);
		else msg.AppendStr("(?)", &msg);
	}

	msg.AppendStr("\n", &msg);
	NUCLEO_USART_vCOM_Write(&msg);
}

/**
 * @brief Shows demagnetization statistics
 * @retval None
 */
void STEST01A1_DIAG_demag_stat() {
	double elapsed_time = DEMAG_TICKS / 16e6;
	double v_demag = 48;
	double v_init = 24;
	double r = 12;

	uint32_t inductance = elapsed_time * r / log((v_demag + v_init) / v_demag);

	msg.AppendStr("Demagnetization duration: ", &msg);
	msg.AppendFloat(elapsed_time * 1000, &msg);
	msg.AppendStr(" ms, ", &msg);
	msg.AppendStr("Inductance: ", &msg);
	msg.AppendFloat(inductance, &msg);
	msg.AppendStr(" H", &msg);
	NUCLEO_USART_vCOM_WriteLine(&msg);
	msg.Reset(&msg);
	DEMAG_TICKS = 0;
}

/**
 * @brief Prints voltage and logical levels of all devices
 * @retval None
 */
void STEST01A1_DIAG_levels() {
	int dev_count = sizeof(devices)/sizeof(STEST01A1_DIAG_DeviceTypeDef);

	for(int i = 0; i < dev_count; i += 1)
		STEST01A1_DIAG_read(devices[i], numerical);
}

/**
 * @brief Prints switch states of all devices
 * @note '?' output symbol means switch state can not be determined
 * @retval None
 */
void STEST01A1_DIAG_states() {
	int dev_count = sizeof(devices)/sizeof(STEST01A1_DIAG_DeviceTypeDef);

	for(int i = 0; i < dev_count; i += 1)
		STEST01A1_DIAG_read(devices[i], logical);
}
