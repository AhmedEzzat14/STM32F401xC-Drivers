/*
 * Serial2Parallel_drivers.c
 *
 * Created on: Aug 30, 2025
 * Author: Ahmed Ezzat
 */

//------------------------------
// Includes
//------------------------------
#include "Serial2Parallel_drivers.h"

//============================================================

/*
 * =======================================================
 * 					  Generic Variables
 * =======================================================
 */
GPIO_TypeDef *G_GPIOx_data, *G_GPIOx_shift, *G_GPIOx_latch;

//============================================================

/*
 * =======================================================
 * 					  Helper Functions
 * =======================================================
 */
static void Serial2Parallel_ShiftData(S2P_PinConfig_t *S2P_pin);
static void Serial2Parallel_LatchData(S2P_PinConfig_t *S2P_pin);

//============================================================

/*
 * ==================================================
 * 		 APIs Supported by "MCAL GPIO DRIVER"
 * ==================================================
 */
void HAL_Serial2Parallel_Init(GPIO_TypeDef *GPIOx_data, GPIO_TypeDef *GPIOx_shift, GPIO_TypeDef *GPIOx_latch, S2P_PinConfig_t *S2P_pin){
	G_GPIOx_data = GPIOx_data;
	G_GPIOx_shift = GPIOx_shift;
	G_GPIOx_latch = GPIOx_latch;

	GPIO_PinConfig_t data;
	data.GPIO_PinNumber = S2P_pin->S2P_Data_Pin;
	data.GPIO_MODE = GPIO_MODE_OP;
	data.GPIO_TYPE = GPIO_TYPE_PP;
	data.GPIO_Output_Speed = GPIO_SPEED_LOW;
	data.GPIO_PU_PD = GPIO_PU_PD_NONE;

	GPIO_PinConfig_t shift;
	shift.GPIO_PinNumber = S2P_pin->S2P_ShiftCLK_Pin;
	shift.GPIO_MODE = GPIO_MODE_OP;
	shift.GPIO_TYPE = GPIO_TYPE_PP;
	shift.GPIO_Output_Speed = GPIO_SPEED_LOW;
	shift.GPIO_PU_PD = GPIO_PU_PD_NONE;

	GPIO_PinConfig_t latch;
	latch.GPIO_PinNumber = S2P_pin->S2P_LatchCLK_Pin;
	latch.GPIO_MODE = GPIO_MODE_OP;
	latch.GPIO_TYPE = GPIO_TYPE_PP;
	latch.GPIO_Output_Speed = GPIO_SPEED_LOW;
	latch.GPIO_PU_PD = GPIO_PU_PD_NONE;

	MCAL_GPIO_Init(G_GPIOx_data, &data);
	MCAL_GPIO_Init(G_GPIOx_shift, &shift);
	MCAL_GPIO_Init(G_GPIOx_latch, &latch);
}

void HAL_Serial2Parallel_SendData(S2P_PinConfig_t *S2P_pin, uint32_t data_byte){
	for(uint8_t i = 0; i < 8 * Number_S2P_Used; i++){
		MCAL_GPIO_WritePin(G_GPIOx_shift, S2P_pin->S2P_ShiftCLK_Pin, GET_BIT(data_byte, i));
		Serial2Parallel_ShiftData(S2P_pin);
	}
	Serial2Parallel_LatchData(S2P_pin);
}

static void Serial2Parallel_ShiftData(S2P_PinConfig_t *S2P_pin){
	MCAL_GPIO_WritePin(G_GPIOx_shift, S2P_pin->S2P_ShiftCLK_Pin, 1);
	MCAL_SysTicK_SetDelay_ms(1);
	MCAL_GPIO_WritePin(G_GPIOx_shift, S2P_pin->S2P_ShiftCLK_Pin, 0);
}

static void Serial2Parallel_LatchData(S2P_PinConfig_t *S2P_pin){
	MCAL_GPIO_WritePin(G_GPIOx_latch, S2P_pin->S2P_LatchCLK_Pin, 1);
	MCAL_SysTicK_SetDelay_ms(1);
	MCAL_GPIO_WritePin(G_GPIOx_latch, S2P_pin->S2P_LatchCLK_Pin, 0);
}
