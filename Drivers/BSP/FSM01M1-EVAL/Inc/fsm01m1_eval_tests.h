/**
 ******************************************************************************
 * @file    fsm01m1_eval_tests.h
 * @author  ST Power Application Lab Prague
 * @version V1.0.0
 * @date    Jan 1, 2023
 * @brief   PCB test routines and procedures
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FSM01M1_EVAL_TESTS_H_
#define FSM01M1_EVAL_TESTS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdbool.h"

/* Exported Constants --------------------------------------------------------*/

/* Exported Types  -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* TEST_PROC procedures */
void FSM01M1_TEST_PROC_check_expected_voltages(bool * vcc, bool * vcc1, bool * out1, bool * vcc2, bool * out2);

/* TEST_CASE routines */
void FSM01M1_TEST_CASE_overcurrent_cutoff_test(void);
void FSM01M1_TEST_CASE_simultaneous_demagnetization(void);
void FSM01M1_TEST_CASE_SFTY_OFF(void);
void FSM01M1_TEST_CASE_loss_off_GND(void);
void FSM01M1_TEST_CASE_EMC_test_routine(void);
void FSM01M1_TEST_CASE_VCCx_test_pulse_routine(void);
void FSM01M1_TEST_CASE_expected_voltages_test(bool vcc, bool vcc1, bool out1, bool vcc2, bool out2);


#ifdef	 __cplusplus
}
#endif

#endif /* FSM01M1_EVAL_TESTS_H_ */


