/*
 * SevenSegment_driver.h
 *
 * Created on: Aug 18, 2025
 * Author: Ahmed Ezzat
 */

#ifndef ECU_INC_SEVENSEGMENT_DRIVER_H_
#define ECU_INC_SEVENSEGMENT_DRIVER_H_

//------------------------------
// Includes
//------------------------------
#include "stm32f401xc.h"
#include "stm32f401xc_gpio_driver.h"

//------------------------------------------
// User Type Definitions (Structures)
//------------------------------------------
typedef enum{
	disable,
	enable
}BCD_Status_t;

//=========================================================

/*
 * =========================================================
 * 		 APIs Supported by "HAL Seven Segment DRIVER"
 * =========================================================
 */

void HAL_SEVEN_SEG_WriteNumber_CommAnode(uint16_t Number, BCD_Status_t bcd_status);
void HAL_SEVEN_SEG_WriteNumber_CommCathode(uint16_t Number, BCD_Status_t bcd_status);

#endif /* ECU_INC_SEVENSEGMENT_DRIVER_H_ */
