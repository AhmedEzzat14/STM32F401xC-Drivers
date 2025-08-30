/*
 * Serial2Parallel_drivers.h
 *
 * Created on: Aug 30, 2025
 * Author: Ahmed Ezzat
 */

#ifndef ECU_INC_SERIAL2PARALLEL_DRIVERS_H_
#define ECU_INC_SERIAL2PARALLEL_DRIVERS_H_

//------------------------------
// Includes
//------------------------------
#include "stm32f401xc_gpio_driver.h"
#include "stm32f401xc_SysTick_driver.h"

//============================================================

//------------------------------------------
// User Type Definitions (Structures)
//------------------------------------------
typedef struct{
	uint8_t	S2P_Data_Pin        ;

	uint8_t	S2P_ShiftCLK_Pin   ;

	uint8_t	S2P_LatchCLK_Pin   ;
}S2P_PinConfig_t;

//============================================================

#define Number_S2P_Used		2

/*
 * ==================================================
 * 		 APIs Supported by "MCAL GPIO DRIVER"
 * ==================================================
 */

void HAL_Serial2Parallel_Init(GPIO_TypeDef *GPIOx_data, GPIO_TypeDef *GPIOx_shift, GPIO_TypeDef *GPIOx_latch, S2P_PinConfig_t *S2P_pin);
void HAL_Serial2Parallel_SendData(S2P_PinConfig_t *S2P_pin, uint32_t data_byte);

#endif /* ECU_INC_SERIAL2PARALLEL_DRIVERS_H_ */
