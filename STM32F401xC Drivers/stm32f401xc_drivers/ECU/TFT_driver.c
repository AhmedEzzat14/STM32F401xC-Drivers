/*
 * TFT_driver.c
 *
 * Created on: Sep 6, 2025
 * Author: Ahmed Ezzat
 */

#include "TFT_driver.h"



GPIO_PinConfig_t *G_Pin_Config = NULL;
GPIO_TypeDef *G_GPIOx_PIN = NULL;

GPIO_PinConfig_t *G_RST_Config = NULL;
GPIO_TypeDef *G_GPIOx_RST = NULL;

static uint16_t CurrentX_Start, CurrentX_End;
static uint16_t CurrentY_Start, CurrentY_End;

static void Write_CMD(uint16_t cmd){
	MCAL_GPIO_WritePin(G_GPIOx_PIN, G_Pin_Config->GPIO_PinNumber, 0);
	MCAL_SPI_Transceive(&cmd, Polling_Enable);
}

static void Write_Data(uint16_t data){
	MCAL_GPIO_WritePin(G_GPIOx_PIN, G_Pin_Config->GPIO_PinNumber, 0);
	MCAL_SPI_Transceive(&data, Polling_Enable);
}

static void TFT_Reset(void){
	MCAL_GPIO_WritePin(G_GPIOx_RST, G_RST_Config->GPIO_PinNumber, 1);
	MCAL_SysTicK_SetDelay_us(100);

	MCAL_GPIO_WritePin(G_GPIOx_RST, G_RST_Config->GPIO_PinNumber, 0);
	MCAL_SysTicK_SetDelay_us(1);

	MCAL_GPIO_WritePin(G_GPIOx_RST, G_RST_Config->GPIO_PinNumber, 1);
	MCAL_SysTicK_SetDelay_us(100);

	MCAL_GPIO_WritePin(G_GPIOx_RST, G_RST_Config->GPIO_PinNumber, 0);
	MCAL_SysTicK_SetDelay_us(100);

	MCAL_GPIO_WritePin(G_GPIOx_RST, G_RST_Config->GPIO_PinNumber, 1);
	MCAL_SysTicK_SetDelay_us(120);
}

void HAL_TFT_Init(GPIO_TypeDef *GPIOx_PIN, GPIO_TypeDef *GPIOx_RST,
				  GPIO_PinConfig_t *Pin_Config, GPIO_PinConfig_t *RST_Config,
				  SPI_Typedef *SPIx, SPI_PinConfig_t *SPI_Config){
	G_Pin_Config = Pin_Config;
	G_GPIOx_PIN = GPIOx_PIN;

	G_RST_Config = RST_Config;
	G_GPIOx_RST = GPIOx_RST;

	MCAL_GPIO_Init(GPIOx_PIN, Pin_Config);
	MCAL_GPIO_Init(GPIOx_RST, RST_Config);

	MCAL_SPI_Init(SPIx, SPI_Config);

	SysTick_Config_t SysTick_config;
	SysTick_config.SysTick_InterruptEnable = SysTick_Interrupt_DIS;
	SysTick_config.SysTick_CLKSource = SysTick_CLK_AHB_8;
	MCAL_SysTicK_Init(&SysTick_config);

	// Reset Sequence
	TFT_Reset();

	// Sleep Out Mode
	Write_CMD(0x11);

	// Wait 15ms
	MCAL_SysTicK_SetDelay_ms(15);

	// Send Color Command -> RGB565 ->0x05
	Write_CMD(0x3A);
	Write_Data(0x05);

	// Display on
	Write_CMD(0x29);
}

void HAL_TFT_ShowImage(uint16_t ImageArray[], uint16_t ImageSize){
	uint16_t MSB = 0;
	uint16_t LSB = 0;

	// Set X Position
	Write_CMD(0x2A);

	// send X Start
	Write_Data(0);
	Write_Data(0);

	// Send X End
	Write_Data(0);
	Write_Data(127);

	// Set Y Position
	Write_CMD(0x2B);

	//  Send Y Start
	Write_Data(0);
	Write_Data(0);

	// Send Y End
	Write_Data(0);
	Write_Data(159);

	// Send Image CMD
	Write_CMD(0x2C);

	// Send Image Data
	for(uint16_t i = 0; i < ImageSize; i++){
		MSB = (ImageArray[i]& 0xff00)>>8 ;

		LSB = ImageArray[i] & 0x00ff;

		Write_Data(MSB);
		Write_Data(LSB);
	}
}

void HAL_TFT_SetXPos(uint16_t X_Start, uint16_t X_End){
	CurrentX_Start = X_Start;
	CurrentX_End = X_End;

	// Set X Position
	Write_CMD(0x2A);

	// Send X Start
	Write_Data(0);
	Write_Data(X_Start);

	// Send X End
	Write_Data(0);
	Write_Data(X_End);
}

void HAL_TFT_SetYPos(uint16_t Y_Start, uint16_t Y_End){
	CurrentY_Start = Y_Start;
	CurrentY_End = Y_End;

	// Set Y Position
	Write_CMD(0x2B);

	// Send Y Start
	Write_Data(0);
	Write_Data(Y_Start);

	// Send Y End
	Write_Data(0);
	Write_Data(Y_End);
}

void HAL_TFT_FillBackground(uint16_t Background_Color){
	HAL_TFT_SetXPos(0,127);

	HAL_TFT_SetYPos(0, 159);

	HAL_TFT_FillRectangle(Background_Color);
}

void HAL_TFT_FillRectangle(uint16_t Rectangle_Color){
	uint16_t MSB = 0;
	uint16_t LSB = 0;

	uint16_t Pixels = (CurrentX_End-CurrentX_Start)*(CurrentY_End-CurrentY_Start);

	// Send Color CMD
	Write_CMD(0x2C);

	// Send Color Data
	for(uint16_t i = 0; i < Pixels; i++){

		MSB = (Rectangle_Color& 0xff00)>>8 ;

		LSB = Rectangle_Color & 0x00ff;
		Write_Data(MSB);
		Write_Data(LSB);
	}
}


