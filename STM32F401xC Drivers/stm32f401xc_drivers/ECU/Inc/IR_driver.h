/*
 * IR_driver.h
 *
 * Created on: Aug 31, 2025
 * Author: Ahmed Ezzat
 */

#ifndef ECU_INC_IR_DRIVER_H_
#define ECU_INC_IR_DRIVER_H_

//------------------------------
// Includes
//------------------------------
#include "stm32f401xc_SysTick_driver.h"
#include "stm32f401xc_gpio_driver.h"

//============================================================

/*
 * ==================================================
 * 		 APIs Supported by "IR DRIVER"
 * ==================================================
 */

void HAL_IR_GetTime(void);
void HAL_IR_DecodeBits(void);
uint8_t get_value(void);

#endif /* ECU_INC_IR_DRIVER_H_ */
