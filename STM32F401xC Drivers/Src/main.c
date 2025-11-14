/**
 *******************************************************************************
 * @file           : main.c
 * @author         : Ahmed
 * @brief          : MPU6050 + UART example
 *******************************************************************************
 */

#include "stm32f401xc_FPU_driver.h"
#include "stm32f401xc_FMI_driver.h"
#include "stm32f401xc_SysTick_driver.h"
#include "stm32f401xc_RCC_driver.h"
#include "stm32f401xc_gpio_driver.h"
#include "stm32f401xc_EXTI_driver.h"
#include "stm32f401xc_USART_driver.h"
#include "stm32f401xc_SPI_driver.h"
#include "stm32f401xc_I2C_driver.h"
#include "stm32f401xc_Timer_driver.h"
#include "IR_driver.h"
#include "Ultrasonic_driver.h"
#include "MPU6050_Driver.h"
#include <stdio.h>

void Clock_Init(void){
    RCC_GPIOA_CLK_EN();
    RCC_GPIOB_CLK_EN();
    RCC_SYSCFG_CLK_EN();
}

int main(void){
    MCAL_FPU_Enable();
    MCAL_RCC_Init();
    Clock_Init();

    USART_PinConfig_t uart;
    uart.USART_Mode = UART_MODE_TX_RX;
    uart.USART_BaudRate = 9600;
    uart.USART_IRQ_Enable = UART_IRQ_ENABLE_NONE;
    uart.USART_StopBits = UART_StopBits_1Bit;
    uart.USART_Sampling = UART_Sampling_16;
    uart.USART_HW_FlowCTRL = UART_HW_FLW_CTRL_RTS_DIS;
    uart.USART_ParityMode = UART_Parity_DIS;
    uart.USART_PayLoad_Lenght = UART_PayLoad_Length_8Bits;
    uart.P_IRQ_CallBack = NULL; MCAL_UART_Init(USART6, &uart);

	I2C_InitTypedef I2C1CFG ;

	//I2C Controller act as a Master

	I2C1CFG.I2C_General_Call_Address = I2C_General_Addres_EN ;
	I2C1CFG.I2C_ACK_Control =I2C_Ack_EN ;
	I2C1CFG.I2C_CLK_Speed = I2C_SCLK_SM_100K ;
	I2C1CFG.I2C_Mode = I2C_Mode_I2C ;
	I2C1CFG.P_Slave_Event_CallBack = NULL ;
	I2C1CFG.P_Master_Event_CallBack = NULL ;
	I2C1CFG.I2C_CLK_StretchMode = I2C_CLK_StretchMode_EN;

    float32 Yaw, Pitch, Roll;
    HAL_MPU6050_Init(I2C2, &I2C1CFG);
    uint8_t buffer[100];

    while(1){
	//HAL_MPU6050_Calculate_Angles();
	// Yaw = HAL_MPU6050_YawVal();
	// Pitch = HAL_MPU6050_PitchVal();
	// Roll = HAL_MPU6050_RollVal();
	// sprintf((char*)buffer, "Encoder: %ld\r\n", MCAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7), MCAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));
	// // //sprintf((char*)buffer, "Yaw: %.2f | Pitch: %.2f | Roll: %.2f\r\n", Yaw, Pitch, Roll);
	MCAL_UART_SendString(USART6, buffer, Polling_Enable);
	// // MCAL_UART_SendData(USART6, &buffer, Polling_Enable);
	} return 0;
}
