/*
 * Led_Matrix_driver.c
 *
 * Created on: Aug 26, 2025
 * Author: Ahmed Ezzat
 */

#include "Led_Matrix_driver.h"


static void HAL_LedMatrix_EnableCurrentCol(uint8_t col_number);
static void HAL_LedMatrix_SetRowValue(uint8_t row_value);
static void HAL_LedMatrix_DisableAllCol(void);

GPIO_TypeDef *G_GPIOx_rows, *G_GPIOx_cols;
GPIO_PinConfig_t *G_rows, *G_cols;
uint8_t G_rows_number, G_cols_number;

void HAL_LedMatrix_Init(GPIO_TypeDef *GPIOx_rows, GPIO_TypeDef *GPIOx_cols,
						GPIO_PinConfig_t *Rows, uint8_t row_number,
						GPIO_PinConfig_t *Cols, uint8_t Col_number){

	G_GPIOx_rows = GPIOx_rows;
	G_GPIOx_cols = GPIOx_cols;
	G_rows = Rows;
	G_cols = Cols;
	G_rows_number = row_number;
	G_cols_number = Col_number;

	for(uint8_t i = 0; i < G_rows_number; i++){
		MCAL_GPIO_Init(G_GPIOx_rows, &Rows[i]);
	}

	for(uint8_t i = 0; i < G_cols_number; i++){
		MCAL_GPIO_Init(G_GPIOx_cols, &Cols[i]);
	}

	// SysTick Init
	SysTick_Config_t SysTick_CFG;

	SysTick_CFG.SysTick_InterruptEnable = SysTick_Interrupt_DIS;
	SysTick_CFG.SysTick_CLKSource = SysTick_CLK_AHB_8;
	MCAL_SysTicK_Init(&SysTick_CFG);
}

void HAL_LedMatrix_DisplayFrame(uint8_t frame[], uint32_t FrameDelay){
	for(uint32_t j = 0; j < FrameDelay; j++){

		for(uint8_t i = 0; i < G_cols_number; i++){

			// Set Row Value
			HAL_LedMatrix_SetRowValue(frame[i]);

			// Enable Current Col
			HAL_LedMatrix_EnableCurrentCol(i);

			MCAL_SysTicK_SetDelay_ms(ScanTime);

			// Disable All Cols
			HAL_LedMatrix_DisableAllCol();
		}
	}
}

static void HAL_LedMatrix_EnableCurrentCol(uint8_t col_number){
	MCAL_GPIO_WritePin(G_GPIOx_cols, G_cols[G_cols_number].GPIO_PinNumber, 0);
}

static void HAL_LedMatrix_SetRowValue(uint8_t row_value){
	for(uint8_t i = 0; i< G_rows_number; i++){
		MCAL_GPIO_WritePin(G_GPIOx_rows, G_rows[i].GPIO_PinNumber, GET_BIT(row_value,i));
	}
}

static void HAL_LedMatrix_DisableAllCol(void){
	for(uint8_t i = 0; i< G_cols_number; i++){
		MCAL_GPIO_WritePin(G_GPIOx_cols, G_cols[i].GPIO_PinNumber, 1);
	}
}
