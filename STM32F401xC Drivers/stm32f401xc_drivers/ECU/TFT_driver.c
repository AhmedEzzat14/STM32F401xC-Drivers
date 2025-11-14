/*
 * TFT_driver.c
 *
 * Created on: Sep 6, 2025
 * Author: Ahmed Ezzat
 */

#include "TFT_driver.h"


GPIO_PinConfig_t *G_Pin_Config  = NULL;
GPIO_TypeDef     *G_GPIOx_PIN   = NULL;

GPIO_PinConfig_t *G_RST_Config  = NULL;
GPIO_TypeDef     *G_GPIOx_RST   = NULL;

static uint16_t CurrentX_Start, CurrentX_End;
static uint16_t CurrentY_Start, CurrentY_End;


static void Write_CMD(uint8_t cmd){
    MCAL_GPIO_WritePin(G_GPIOx_PIN, G_Pin_Config->GPIO_PinNumber, 0);
    MCAL_SPI_Transceive(&cmd, Polling_Enable);
}

static void Write_Data(uint8_t data){
    MCAL_GPIO_WritePin(G_GPIOx_PIN, G_Pin_Config->GPIO_PinNumber, 1);
    MCAL_SPI_Transceive(&data, Polling_Enable);
}

static void TFT_Reset(void){
    MCAL_GPIO_WritePin(G_GPIOx_RST, G_RST_Config->GPIO_PinNumber, 1);
    MCAL_SysTicK_SetDelay_us(150);
    MCAL_GPIO_WritePin(G_GPIOx_RST, G_RST_Config->GPIO_PinNumber, 0);
    MCAL_SysTicK_SetDelay_us(2);
    MCAL_GPIO_WritePin(G_GPIOx_RST, G_RST_Config->GPIO_PinNumber, 1);
    MCAL_SysTicK_SetDelay_us(150);
    MCAL_GPIO_WritePin(G_GPIOx_RST, G_RST_Config->GPIO_PinNumber, 0);
    MCAL_SysTicK_SetDelay_us(100);
    MCAL_GPIO_WritePin(G_GPIOx_RST, G_RST_Config->GPIO_PinNumber, 1);
    MCAL_SysTicK_SetDelay_us(200000);

    Write_CMD(0x11);
    MCAL_SysTicK_SetDelay_us(200000);
}

void HAL_TFT_Init(GPIO_TypeDef *GPIOx_PIN, GPIO_TypeDef *GPIOx_RST, GPIO_PinConfig_t *Pin_Config, GPIO_PinConfig_t *RST_Config, SPI_Typedef *SPIx, SPI_PinConfig_t *SPI_Config){
    G_Pin_Config = Pin_Config;
    G_GPIOx_PIN  = GPIOx_PIN;

    G_RST_Config = RST_Config;
    G_GPIOx_RST  = GPIOx_RST;

    MCAL_GPIO_Init(GPIOx_PIN, Pin_Config);
    MCAL_GPIO_Init(GPIOx_RST, RST_Config);
    MCAL_SPI_Init(SPIx, SPI_Config);

    SysTick_Config_t SysTick_config;
    SysTick_config.SysTick_InterruptEnable = SysTick_Interrupt_DIS;
    SysTick_config.SysTick_CLKSource       = SysTick_CLK_AHB_8;
    MCAL_SysTicK_Init(&SysTick_config);

    // Reset and configure TFT
    TFT_Reset();
    Write_CMD(0x11);
    MCAL_SysTicK_SetDelay_ms(15);

    // Set RGB565 color mode
    Write_CMD(0x3A);
    Write_Data(0x05);

    // Display ON
    Write_CMD(0x29);
}

void HAL_TFT_SetXPos(uint16_t X_Start, uint16_t X_End){
    CurrentX_Start = X_Start;
    CurrentX_End   = X_End;

    Write_CMD(0x2A);
    Write_Data(0);
    Write_Data(X_Start);
    Write_Data(0);
    Write_Data(X_End);
}

void HAL_TFT_SetYPos(uint16_t Y_Start, uint16_t Y_End){
    CurrentY_Start = Y_Start;
    CurrentY_End   = Y_End;

    Write_CMD(0x2B);
    Write_Data(0);
    Write_Data(Y_Start);
    Write_Data(0);
    Write_Data(Y_End);
}

void HAL_TFT_FillBackground(uint16_t Background_Color){
    HAL_TFT_SetXPos(0, 127);
    HAL_TFT_SetYPos(0, 159);
    HAL_TFT_FillRectangle(Background_Color);
}

void HAL_TFT_FillRectangle(uint16_t Rectangle_Color){
    uint16_t MSB, LSB;
    uint32_t Pixels = (CurrentX_End - CurrentX_Start + 1) *
	    (CurrentY_End - CurrentY_Start + 1);

    Write_CMD(0x2C);

    for (uint32_t i = 0; i < Pixels; i++)
	{
	MSB = (Rectangle_Color >> 8);
	LSB = (Rectangle_Color & 0xFF);
	Write_Data(MSB);
	Write_Data(LSB);
	}
    }

static void HAL_TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
    {
    HAL_TFT_SetXPos(x, x);
    HAL_TFT_SetYPos(y, y);
    Write_CMD(0x2C);
    Write_Data(color >> 8);
    Write_Data(color & 0xFF);
    }

void HAL_TFT_ShowString(uint16_t x, uint16_t y, const char *str, uint16_t textColor, uint16_t bgColor, uint8_t size){
    uint16_t cursorX = x;
    uint16_t cursorY = y;

    while (*str){
	char c = *str++;

	if (c == '\n'){
	    cursorY += (8 * size);
	    cursorX = x;
	    continue;
	}

	const uint8_t *bitmap = NULL;

	if (c == ' ')
	    bitmap = font_space;
	else if (c >= '0' && c <= '9')
	    bitmap = font_digits[c - '0'];
	else if (c >= 'A' && c <= 'Z')
	    bitmap = font_uppercase[c - 'A'];
	else if (c >= 'a' && c <= 'z')
	    bitmap = font_lowercase[c - 'a'];
	else if (c >= '!' && c <= '/')
	    bitmap = font_punct[c - '!'];

	if (bitmap){
	    for (uint8_t row = 0; row < 7; row++){
		for (uint8_t col = 0; col < 5; col++){
		    uint16_t color = (bitmap[col] & (1 << row)) ? textColor : bgColor;

		    for (uint8_t dy = 0; dy < size; dy++){
			for (uint8_t dx = 0; dx < size; dx++){
			    HAL_TFT_DrawPixel(cursorX + col * size + dx, cursorY + row * size + dy, color);
			}
		    }
		}
	    }
	}

	cursorX += (6 * size);
    }
}

void HAL_TFT_ShowImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *image){
    // Define full image window
    HAL_TFT_SetXPos(x, x + width - 1);
    HAL_TFT_SetYPos(y, y + height - 1);
    Write_CMD(0x2C);

    // Draw pixel data row by row
    for (uint32_t i = 0; i < (uint32_t)(width * height); i++){
	uint16_t color = image[i];
	Write_Data(color >> 8);
	Write_Data(color & 0xFF);
    }
}
