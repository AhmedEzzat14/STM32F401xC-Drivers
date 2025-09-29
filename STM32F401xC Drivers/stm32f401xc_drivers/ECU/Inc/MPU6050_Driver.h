/*
 * MPU6050_Driver.h
 *
 * Created on: Sep 23, 2025
 * Author: Ahmed Ezzat
 */

#ifndef ECU_INC_MPU6050_DRIVER_H_
#define ECU_INC_MPU6050_DRIVER_H_

//------------------------------
// Includes
//------------------------------
#include "stm32f401xc_I2C_driver.h"
#include "stm32f401xc_SysTick_driver.h"
#include <math.h>

//============================================================

/*
 * =======================================================
 * 					  Generic Macros
 * =======================================================
 */
#define MPU6050_CLK							8000000

#define MPU6050_Address_PWR_Low  			0x68
#define MPU6050_Address_PWR_High			0x69

#define MPU6050_WHO_AM_I_REG    		 	0x75
#define MPU6050_SMPLRT_DIV_REG   			0x19
#define MPU6050_GYRO_CONFIG_REG  			0x1B
#define MPU6050_ACCEL_CONFIG_REG 			0x1C
#define MPU6050_ACCEL_XOUT_H_REG 			0x3B
#define MPU6050_TEMP_OUT_H_REG   			0x41
#define MPU6050_GYRO_XOUT_H_REG 			0x43
#define MPU6050_PWR_MGMT_1_REG  			0x6B

//============================================================

/*
 * =================================================
 * 		  APIs Supported by "MPU6050 DRIVER"
 * =================================================
 */
void HAL_MPU6050_Init(I2C_TypeDef * I2Cx, I2C_InitTypedef *I2C_Config);

void HAL_MPU6050_Write(uint8_t RegAdd, uint8_t *Data, uint8_t DataLen);
void HAL_MPU6050_Read(uint8_t RegAdd, uint8_t *Data, uint8_t DataLen);

void HAL_MPU6050_Calculate_Angles(void);

uint8_t HAL_MPU6050_PitchVal(void);
uint8_t HAL_MPU6050_YawVal(void);
uint8_t HAL_MPU6050_RollVal(void);

#endif /* ECU_INC_MPU6050_DRIVER_H_ */
