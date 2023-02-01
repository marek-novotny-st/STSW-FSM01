/*
 * STEVAL-FSM01M1-diagnostic_driver.c
 *
 *  Created on: 28. 11. 2022
 *      Author: marek novotny
 */

/* Includes ------------------------------------------------------------------*/
#include <fsm01m1_eval_diagnostic_driver.h>
#include <fsm01m1_eval_driver.h>
#include <fsm01m1_eval_usart_driver.h>
#include "string.h"

/* Private types -------------------------------------------------------------*/
typedef enum USART_Format_t {
	numerical,
	logical
} USART_FormatTypeDef;

/* Private variables ---------------------------------------------------------*/
float VCC_ADC;
float VCC1_ADC;
float VCC2_ADC;
float OUT1_ADC;
float OUT2_ADC;

/* buffer for command tokenization */
char * token_ctx;

/* registered devices */
DIAG_DeviceTypeDef devices[15] = {
		vcc,
		vcc1,
		vcc1_dsc,
		vcc2,
		vcc2_dsc,
		out1,
		out1_dsc,
		out2,
		out2_dsc,
		in1,
		in2,
		coff1,
		coff2,
		tp1,
		tp2
};

DIAG_DeviceTypeDef outputs[2] = { out1, out2 };

DIAG_DeviceTypeDef sources[3] = { vcc, vcc1, vcc2 };

/* registered actions */
DIAG_ActionTypeDef actions[7] = {
		on,
		off,
		read,
		state,
		states,
		level,
		levels
};

/* Private function prototypes -----------------------------------------------*/
void FSM01M1_DIAG_read(DIAG_DeviceTypeDef dev, USART_FormatTypeDef fmt);
void FSM01M1_DIAG_reset_devices();
void FSM01M1_DIAG_switch(DIAG_DeviceTypeDef dev, DIAG_ActionTypeDef act);
void FSM01M1_DIAG_resolve(char * cmd, DIAG_DeviceTypeDef target);
void FSM01M1_DIAG_levels();
void FSM01M1_DIAG_states();

/**
 * @brief Prints starting message
 * @retval None
 */
void FSM01M1_DIAG_splash_msg() {
	USART_MessageTypeDef msg = FSM01M1_USART_vCOM_CreateMessage();
	msg.AppendStr("***** STEVAL-FSM01M1 DIAGNOSTIC TOOL *****\n", &msg);
	msg.AppendStr("* Type help for usage information", &msg);
	FSM01M1_USART_vCOM_WriteLine(&msg);
}

/**
 * @brief Prints help message
 * @retval None
 */
void FSM01M1_DIAG_help() {
	USART_MessageTypeDef msg = FSM01M1_USART_vCOM_CreateMessage();
	msg.AppendStr("[device] [action] - applies action to a device\n"
			"[global_action] - applies action to all devices\n"
			"- Type 'devices' for device list\n"
			"- Type 'actions' for action list\n"
			/*"- Use 'x' in device identifiers for numerical wildcard (e.g. outx selects all outputs)\n"*/, &msg);
	FSM01M1_USART_vCOM_Write(&msg);
}

/**
 * @brief Prints list of devices
 * @retval None
 */
void FSM01M1_DIAG_list_devices() {
	int dev_count = sizeof(devices)/sizeof(DIAG_DeviceTypeDef);

	USART_MessageTypeDef msg = FSM01M1_USART_vCOM_CreateMessage();
	for (int i = 0; i < dev_count; i += 1) {
		switch (devices[i]) {
			case vcc:
				msg.AppendStr("vcc\n", &msg);
				break;
			case vcc1:
				msg.AppendStr("vcc1\n", &msg);
				break;
			case vcc1_dsc:
				msg.AppendStr("vcc1_dsc\n", &msg);
				break;
			case vcc2:
				msg.AppendStr("vcc2\n", &msg);
				break;
			case vcc2_dsc:
				msg.AppendStr("vcc2_dsc\n", &msg);
				break;
			case in1:
				msg.AppendStr("in1\n", &msg);
				break;
			case in2:
				msg.AppendStr("in2\n", &msg);
				break;
			case out1:
				msg.AppendStr("out1\n", &msg);
				break;
			case out1_dsc:
				msg.AppendStr("out1_dsc\n", &msg);
				break;
			case out2:
				msg.AppendStr("out2\n", &msg);
				break;
			case out2_dsc:
				msg.AppendStr("out2_dsc\n", &msg);
				break;
			case coff1:
				msg.AppendStr("coff1\n", &msg);
				break;
			case coff2:
				msg.AppendStr("coff2\n", &msg);
				break;
			case tp1:
				msg.AppendStr("tp1\n", &msg);
				break;
			case tp2:
				msg.AppendStr("tp2\n", &msg);
				break;
			default:
				break;
		}
	}

	FSM01M1_USART_vCOM_Write(&msg);
}

/**
 * @brief Prints list of actions
 * @retval None
 */
void FSM01M1_DIAG_list_actions() {
	int act_len = sizeof(actions)/sizeof(DIAG_ActionTypeDef);

	USART_MessageTypeDef msg = FSM01M1_USART_vCOM_CreateMessage();
	for (int i = 0; i < act_len; i += 1) {
		switch (actions[i]) {
			case on:
				msg.AppendStr("on\n", &msg);
				break;
			case off:
				msg.AppendStr("off\n", &msg);
				msg.AppendStr("off [global]\n", &msg);
				break;
			case read:
				msg.AppendStr("read\n", &msg);
				break;
			case state:
				msg.AppendStr("state\n", &msg);
				break;
			case states:
				msg.AppendStr("states [global]\n", &msg);
				break;
			case level:
				msg.AppendStr("level\n", &msg);
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
 * @brief Runs a diagnostic IO command line application
 * @param huart: uart handle
 * @retval None
 */
void FSM01M1_DIAG_IO_Loop(UART_HandleTypeDef * huart) {
	FSM01M1_USART_vCOM_Config(huart);
	USART_MessageTypeDef cmd = FSM01M1_USART_vCOM_CreateMessage();

	FSM01M1_DIAG_splash_msg();
	while(1) {
		if (cmd.flag == ready) {
			FSM01M1_DIAG_resolve(cmd.data, all);
			cmd.Clear(&cmd);
			cmd.flag = idle;

			FSM01M1_USART_vCOM_WriteChar('\n');
		}
		if (cmd.flag == idle) {
			FSM01M1_USART_vCOM_ReadLine(&cmd);
		}
	}
}

/**
 * @brief Provides command resolution services
 * @param cmd: command
 * @param target: device
 * @retval None
 */
void FSM01M1_DIAG_resolve(char * cmd, DIAG_DeviceTypeDef target) {
	if (cmd[0] == '\r' || cmd[0] == '\n') cmd = cmd + 1;
	char * arg = strtok_r(cmd, " ", &token_ctx);
	arg[strcspn(arg, "\r\n")] = '\0';

	if (arg == NULL) return;
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
	else if (strcmp(arg, "read") == 0) FSM01M1_DIAG_read(target, numerical);
	else if (strcmp(arg, "state") == 0) FSM01M1_DIAG_read(target, logical);
	else if (strcmp(arg, "level") == 0) FSM01M1_DIAG_read(target, numerical);
	else if (strcmp(arg, "levels") == 0) FSM01M1_DIAG_levels();
	else if (strcmp(arg, "states") == 0) FSM01M1_DIAG_states();
	else if (strcmp(arg, "help") == 0) FSM01M1_DIAG_help();
	else if (strcmp(arg, "devices") == 0) FSM01M1_DIAG_list_devices();
	else if (strcmp(arg, "actions") == 0) FSM01M1_DIAG_list_actions();
	else {
		USART_MessageTypeDef msg = FSM01M1_USART_vCOM_CreateMessage();
		msg.AppendStr("Invalid command, no actions performed", &msg);
		FSM01M1_USART_vCOM_WriteLine(&msg);
	}
}

/**
 * @brief Resets devices to off state
 * @retval None
 */
void FSM01M1_DIAG_reset_devices() {
	FSM01M1_VCC1_OFF();
	FSM01M1_VCC1_DSC_OFF();
	FSM01M1_VCC2_OFF();
	FSM01M1_VCC2_DSC_OFF();
	FSM01M1_OUT1_CTRL_OFF();
	FSM01M1_OUT1_DSC_OFF();
	FSM01M1_OUT2_CTRL_OFF();
	FSM01M1_OUT2_DSC_OFF();
	FSM01M1_CUTOFF1_CTRL_OFF();
	FSM01M1_CUTOFF2_CTRL_OFF();
	FSM01M1_TP1_OFF();
	FSM01M1_TP2_OFF();
}

/**
 * @brief Provides device switching services
 * @param dev: device
 * @param act: action
 * @retval None
 */
void FSM01M1_DIAG_switch(DIAG_DeviceTypeDef dev, DIAG_ActionTypeDef act) {
	switch (dev) {
		case vcc1:
			if (act == on) FSM01M1_VCC1_ON();
			else FSM01M1_VCC1_OFF();
			break;
		case vcc1_dsc:
			if (act == on) FSM01M1_VCC1_DSC_ON();
			else FSM01M1_VCC1_DSC_OFF();
			break;
		case vcc2:
			if (act == on) FSM01M1_VCC2_ON();
			else FSM01M1_VCC2_OFF();
			break;
		case vcc2_dsc:
			if (act == on) FSM01M1_VCC2_DSC_ON();
			else FSM01M1_VCC2_DSC_OFF();
			break;
		case vccx: {
				int vcc_count = sizeof(sources)/sizeof(DIAG_DeviceTypeDef);
				for (int i = 0; i < vcc_count; i += 1) {
					FSM01M1_DIAG_switch(sources[i], act);
				}
			}
			break;
		case out1:
			if (act == on) FSM01M1_OUT1_CTRL_ON();
			else FSM01M1_OUT1_CTRL_OFF();
			break;
		case out1_dsc:
			if (act == on) FSM01M1_OUT1_DSC_ON();
			else FSM01M1_OUT1_DSC_OFF();
			break;
		case out2:
			if (act == on) FSM01M1_OUT2_CTRL_ON();
			else FSM01M1_OUT2_CTRL_OFF();
			break;
		case out2_dsc:
			if (act == on) FSM01M1_OUT2_DSC_ON();
			else FSM01M1_OUT2_DSC_OFF();
			break;

		case outx: {
				int out_count = sizeof(outputs)/sizeof(DIAG_DeviceTypeDef);
				for (int i = 0; i < out_count; i += 1) {
					FSM01M1_DIAG_switch(outputs[i], act);
				}
			}
			break;
		case coff1:
			if (act == on) FSM01M1_CUTOFF1_CTRL_ON();
			else FSM01M1_CUTOFF1_CTRL_OFF();
			break;
		case coff2:
			if (act == on) FSM01M1_CUTOFF2_CTRL_ON();
			else FSM01M1_CUTOFF2_CTRL_OFF();
			break;
		case tp1:
			if (act == on) FSM01M1_TP1_ON();
			else FSM01M1_TP1_OFF();
			break;
		case tp2:
			if (act == on) FSM01M1_TP2_ON();
			else FSM01M1_TP2_OFF();
			break;
		case all:
			if (act == off) FSM01M1_DIAG_reset_devices();
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
void FSM01M1_DIAG_read(DIAG_DeviceTypeDef dev, USART_FormatTypeDef fmt) {
	USART_MessageTypeDef msg = FSM01M1_USART_vCOM_CreateMessage();

	int logic = -1;
	float reading = -1.0;

	switch (dev) {
		case vcc:
			reading = FSM01M1_ADC120_read_single_node(&hspi2, VCC_ADC_CHANNEL_ID);
			msg.AppendStr("VCC = ", &msg);
			break;
		case vcc1:
			reading = FSM01M1_ADC120_read_single_node(&hspi2, VCC1_ADC_CHANNEL_ID);
			msg.AppendStr("VCC1 = ", &msg);
			break;
		case vcc1_dsc:
			logic = (int) HAL_GPIO_ReadPin(VCC1_DSC_GPIO_Port, VCC1_DSC_Pin);
			msg.AppendStr("VCC1_DSC = ", &msg);
			break;
		case vcc2:
			reading = FSM01M1_ADC120_read_single_node(&hspi2, VCC2_ADC_CHANNEL_ID);
			msg.AppendStr("VCC2 = ", &msg);
			break;
		case vcc2_dsc:
			logic = (int) HAL_GPIO_ReadPin(VCC2_DSC_GPIO_Port, VCC2_DSC_Pin);
			msg.AppendStr("VCC2_DSC = ", &msg);
			break;
		case in1:
			logic = (int) HAL_GPIO_ReadPin(OUTP1_GPIO_Port, OUTP1_Pin);
			msg.AppendStr("IN1 = ", &msg);
			break;
		case in2:
			logic = (int) HAL_GPIO_ReadPin(OUTP2_GPIO_Port, OUTP2_Pin);
			msg.AppendStr("IN2 = ", &msg);
			break;
		case out1:
			reading = FSM01M1_ADC120_read_single_node(&hspi2, OUT1_ADC_CHANNEL_ID);
			msg.AppendStr("OUT1 = ", &msg);
			break;
		case out1_dsc:
			logic = (int) HAL_GPIO_ReadPin(OUT1_DSC_GPIO_Port, OUT1_DSC_Pin);
			msg.AppendStr("OUT1_DSC = ", &msg);
			break;
		case out2:
			reading = FSM01M1_ADC120_read_single_node(&hspi2, OUT2_ADC_CHANNEL_ID);
			msg.AppendStr("OUT2 = ", &msg);
			break;
		case out2_dsc:
			logic = (int) HAL_GPIO_ReadPin(OUT2_DSC_GPIO_Port, OUT2_DSC_Pin);
			msg.AppendStr("OUT2_DSC = ", &msg);
			break;
		case coff1:
			logic = (int) HAL_GPIO_ReadPin(COFF1_CTRL_GPIO_Port, COFF1_CTRL_Pin) == GPIO_PIN_RESET;
			msg.AppendStr("COFF1 = ", &msg);
			break;
		case coff2:
			logic = (int) HAL_GPIO_ReadPin(COFF2_CTRL_GPIO_Port, COFF2_CTRL_Pin) == GPIO_PIN_RESET;
			msg.AppendStr("COFF2 = ", &msg);
			break;
		case tp1:
			logic = (int) HAL_GPIO_ReadPin(TP1_CTRL_GPIO_Port, TP1_CTRL_Pin) == GPIO_PIN_RESET;
			msg.AppendStr("TP1 = ", &msg);
			break;
		case tp2:
			logic = (int) HAL_GPIO_ReadPin(TP2_CTRL_GPIO_Port, TP2_CTRL_Pin) == GPIO_PIN_RESET;
			msg.AppendStr("TP2 = ", &msg);
			break;
		default:
			break;
	}

	if (fmt == numerical) {
		if (reading != -1.0) msg.AppendFloat(reading, &msg);
		else if (logic != -1) msg.AppendInt(logic, &msg);
	}
	else if (fmt == logical) {
		if (reading >= FSM01M1_NOMINAL_VOLTAGE_THRESHOLD || logic == 1) msg.AppendStr("(on)", &msg);
		else if (reading <= FSM01M1_ZERO_VOLTAGE_THRESHOLD || logic == 0) msg.AppendStr("(off)", &msg);
		else msg.AppendStr("(?)", &msg);
	}

	FSM01M1_USART_vCOM_Write(&msg);
}

/**
 * @brief Prints voltage and logical levels of all devices
 * @retval None
 */
void FSM01M1_DIAG_levels() {
	int dev_count = sizeof(devices)/sizeof(DIAG_DeviceTypeDef);

	for(int i = 0; i < dev_count; i += 1) {
		FSM01M1_DIAG_read(devices[i], numerical);
		if (i < dev_count - 1) {
			FSM01M1_USART_vCOM_WriteChar(' ');
		}
	}
}

/**
 * @brief Prints switch states of all devices
 * @note '?' output symbol means switch state can not be determined
 * @retval None
 */
void FSM01M1_DIAG_states() {
	int dev_count = sizeof(devices)/sizeof(DIAG_DeviceTypeDef);

	for(int i = 0; i < dev_count; i += 1) {
		FSM01M1_DIAG_read(devices[i], logical);
		if (i < dev_count - 1) {
			FSM01M1_USART_vCOM_WriteChar(' ');
		}
	}
}

/**
 * @brief Compares expected voltage levels with real ones
 * @params expected switch states of devices
 * @retval None
 */
void FSM01M1_DIAG_check_expected_voltages(bool * vcc, bool * vcc1, bool * out1, bool * vcc2, bool * out2) {
	FSM01M1_OUT1_CTRL_OFF();
	FSM01M1_VCC1_OFF();
	FSM01M1_OUT2_CTRL_OFF();
	FSM01M1_VCC2_OFF();
	HAL_Delay(100);

	/* Switch */
	if (vcc1) FSM01M1_VCC1_ON();
	if (vcc2) FSM01M1_VCC2_ON();
	if (out1) FSM01M1_OUT1_CTRL_ON();
	if (out2) FSM01M1_OUT2_CTRL_ON();
	HAL_Delay(100);

	FSM01M1_OUT1_CTRL_ON();
	HAL_Delay(100);
	/* ADC measurements */
	VCC1_ADC = FSM01M1_ADC120_read_single_node(&hspi2, VCC_ADC_CHANNEL_ID);
//	VCC1_ADC = STEVAL_FSM01M1_scan_voltage_point(&hspi2, VCC_ADC_CHANNEL_ID);
//	VCC_ADC = STEVAL_FSM01M1_scan_voltage_point(&hspi2, OUT1_ADC_CHANNEL_ID);
	VCC_ADC = FSM01M1_ADC120_read_single_node(&hspi2, OUT1_ADC_CHANNEL_ID);
	OUT1_ADC = FSM01M1_ADC120_read_single_node(&hspi2, VCC2_ADC_CHANNEL_ID);
	VCC2_ADC = FSM01M1_ADC120_read_single_node(&hspi2, OUT2_ADC_CHANNEL_ID);
	OUT2_ADC = FSM01M1_ADC120_read_single_node(&hspi2, VCC1_ADC_CHANNEL_ID);

	/* Criteria */

	// Initialize result by checking Vcc
	bool result = VCC_ADC >= FSM01M1_NOMINAL_VOLTAGE_THRESHOLD;
	(* vcc) = result;


	if (vcc1) {
		bool check = VCC1_ADC >= FSM01M1_NOMINAL_VOLTAGE_THRESHOLD;
		result = result && check;
		(* vcc1) = check;
	}
	else {
		bool check = VCC1_ADC <= FSM01M1_ZERO_VOLTAGE_THRESHOLD;
		result = result && check;
		(* vcc1) = check;
	}
	if (out1) {
		bool check = OUT1_ADC >= FSM01M1_NOMINAL_VOLTAGE_THRESHOLD;
		result = result && check;
		(* out1) = check;
	}
	else {
		bool check = OUT1_ADC <= FSM01M1_ZERO_VOLTAGE_THRESHOLD;
		result = result && check;
		(* out1) = check;
	}
	if (vcc2) {
		bool check = VCC2_ADC >= FSM01M1_NOMINAL_VOLTAGE_THRESHOLD;
		result = result && check;
		(* vcc2) = check;
	}
	else {
		bool check = VCC2_ADC <= FSM01M1_ZERO_VOLTAGE_THRESHOLD;
		result = result && check;
		(* vcc2) = check;
	}
	if (out2) {
		bool check = OUT2_ADC >= FSM01M1_NOMINAL_VOLTAGE_THRESHOLD;
		result = result && check;
		(* out2) = check;
	}
	else {
		bool check = OUT2_ADC <= FSM01M1_ZERO_VOLTAGE_THRESHOLD;
		result = result && check;
		(* out2) = check;
	}
}

/**
 * @brief Provides testing service for expected voltage levels
 * @params expected switch states of devices
 * @retval None
 */
void FSM01M1_DIAG_expected_voltages_test(bool vcc, bool vcc1, bool out1, bool vcc2, bool out2) {
	FSM01M1_DIAG_check_expected_voltages(&vcc, &vcc1, &out1, &vcc2, &out2);

	USART_MessageTypeDef msg = FSM01M1_USART_vCOM_CreateMessage();

	bool result = vcc && vcc1 && out1 && vcc2 && out2;

	// Test VCC
	if (vcc) {
		msg.AppendStr("VCC PASSED ", &msg);
		msg.AppendFloat(VCC_ADC, &msg);
	}
	else {
		msg.AppendStr("VCC FAILED ", &msg);
		msg.AppendFloat(VCC_ADC, &msg);
	}
	FSM01M1_USART_vCOM_FlushWriteLine(&msg);

	// Test VCC1
	if (vcc1) {
		msg.AppendStr("VCC1 PASSED ", &msg);
		msg.AppendFloat(VCC1_ADC, &msg);
	}
	else {
		msg.AppendStr("VCC1 FAILED ", &msg);
		msg.AppendFloat(VCC1_ADC, &msg);
	}
	FSM01M1_USART_vCOM_FlushWriteLine(&msg);

	// Test OUT1
	if (out1) {
		msg.AppendStr("OUT1 PASSED ", &msg);
		msg.AppendFloat(OUT1_ADC, &msg);
	}
	else {
		msg.AppendStr("OUT1 FAILED ", &msg);
		msg.AppendFloat(OUT1_ADC, &msg);
	}
	FSM01M1_USART_vCOM_FlushWriteLine(&msg);

	// Test VCC2
	if (vcc2) {
		msg.AppendStr("VCC2 PASSED ", &msg);
		msg.AppendFloat(VCC2_ADC, &msg);
	}
	else {
		msg.AppendStr("VCC2 FAILED ", &msg);
		msg.AppendFloat(VCC2_ADC, &msg);
	}
	FSM01M1_USART_vCOM_FlushWriteLine(&msg);

	// Test OUT2
	if (out2) {
		msg.AppendStr("OUT2 PASSED ", &msg);
		msg.AppendFloat(OUT2_ADC, &msg);
	}
	else {
		msg.AppendStr("OUT2 FAILED ", &msg);
		msg.AppendFloat(OUT2_ADC, &msg);
	}
	FSM01M1_USART_vCOM_FlushWriteLine(&msg);

	if (!result) {
		FSM01M1_user_LED_red_ON();
		while(1);
	}
}


