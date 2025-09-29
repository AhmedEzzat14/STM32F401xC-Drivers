/*
 * MPU6050_Driver.c
 *
 * Created on: Sep 23, 2025
 * Author: Ahmed Ezzat
 */

//------------------------------
// Includes
//------------------------------
#include "MPU6050_Driver.h"

//============================================================

/*
 * =======================================================
 * 					  Generic Variables
 * =======================================================
 */
I2C_TypeDef *G_MPU6050x = NULL;
I2C_InitTypedef *G_MPU6050_Config = NULL;

float32 yaw,roll,pitch;

//============================================================

/*
 * =================================================
 * 		  APIs Supported by "MPU6050 DRIVER"
 * =================================================
 */
void HAL_MPU6050_Init(I2C_TypeDef *I2Cx, I2C_InitTypedef *I2C_Config){
	G_MPU6050x = I2Cx;
	G_MPU6050_Config = I2C_Config;

	MCAL_I2C_Init(I2Cx, I2C_Config);

	uint8_t DataCheck[2] = {0};
	uint8_t Data = 0;

	HAL_MPU6050_Read(MPU6050_WHO_AM_I_REG, DataCheck, 1);

	if(DataCheck[0] != 104){
		while(1);
	}

	if(DataCheck[0] == 104){
		// Set DATA RATE of 1KHz by Writing SMPLRT_DIV Register
		Data = 0x05;
		HAL_MPU6050_Write(0x1A, &Data, 1);

		// Set Accelerometer Configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ± 8g
		Data = 0x10;
		HAL_MPU6050_Write(MPU6050_ACCEL_CONFIG_REG, &Data, 1);

		// Set Gyroscope	 Configuration in GYRO_CONFIG Register
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ± 500 °/s
		Data = 0x08;
		HAL_MPU6050_Write(MPU6050_GYRO_CONFIG_REG, &Data, 1);

		// Enable the Module
		Data = 0x00;
		HAL_MPU6050_Write(MPU6050_PWR_MGMT_1_REG, &Data, 1);
	}
}

void HAL_MPU6050_Write(uint8_t RegAdd, uint8_t *Data, uint8_t DataLen){
	uint8_t L_DataLen=0;
	uint8_t buffer[3];
	buffer[0] = (uint8_t)(RegAdd);


	for(L_DataLen = 1; L_DataLen < (DataLen+1); L_DataLen++){
		buffer[L_DataLen]= Data[L_DataLen-1];
	}

	MCAL_I2C_Master_Tx(G_MPU6050x, MPU6050_Address_PWR_Low, buffer, DataLen + 1, With_Stop, Start);
}

void HAL_MPU6050_Read(uint8_t RegAdd, uint8_t *Data, uint8_t DataLen){
	uint8_t buffer[2];

	buffer[0] = (uint8_t)(RegAdd);

	MCAL_I2C_Master_Tx(G_MPU6050x, MPU6050_Address_PWR_Low, buffer, DataLen + 1, With_Stop, Start);
	MCAL_I2C_Master_Rx(G_MPU6050x, MPU6050_Address_PWR_Low, Data, DataLen, With_Stop, Repeated_Start);
}

void HAL_MPU6050_Calculate_Angles(void){
	float32 Accel_X_RAW = 0;
	float32 Accel_Y_RAW = 0;
	float32 Accel_Z_RAW = 0;

	uint16_t Gyro_X_RAW = 0;
	uint16_t Gyro_Y_RAW = 0;
	uint16_t Gyro_Z_RAW = 0;

	float32 Gx = 0, Gy = 0, Gz = 0;

	float32 elapsedTime=0.015;
	float32 accAngleX = 0, accAngleY = 0, gyroAngleX = 0, gyroAngleY = 0; //gyroAngleZ = 0;

	uint8_t data[6] = {0};
	uint8_t Data = 0x00;

	HAL_MPU6050_Write(MPU6050_PWR_MGMT_1_REG, &Data, 1);

	Data = 0x05;
	HAL_MPU6050_Write(0x1A, &Data, 1);

	Data = 0x10;
	HAL_MPU6050_Write(MPU6050_ACCEL_CONFIG_REG, &Data, 1);

	// Read acceleration
	HAL_MPU6050_Read(MPU6050_ACCEL_XOUT_H_REG, data, 6);
	Accel_X_RAW = ((uint16_t)(data[0] << 8 | data [1])) / 4096.0;
	Accel_Y_RAW = ((uint16_t)(data[2] << 8 | data [3])) / 4096.0;
	Accel_Z_RAW = ((uint16_t)(data[4] << 8 | data [5])) / 4096.0;

	accAngleX = (atan(Accel_Y_RAW / sqrt(pow(Accel_X_RAW, 2) + pow(Accel_Z_RAW, 2))) * 180 / 3.14) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
	accAngleY = (atan(-1 * Accel_X_RAW / sqrt(pow(Accel_Y_RAW, 2) + pow(Accel_Z_RAW, 2))) * 180 / 3.14) + 1.58; // AccErrorY ~(-1.58)

	// Init GyroScope
	Data = 0x08;
	HAL_MPU6050_Write(MPU6050_GYRO_CONFIG_REG, &Data, 1);

	// Gyro Read
	HAL_MPU6050_Read(MPU6050_GYRO_XOUT_H_REG, data, 6);
	Gyro_X_RAW = (uint16_t)(data[0] << 8 | data [1]);
	Gyro_Y_RAW = (uint16_t)(data[2] << 8 | data [3]);
	Gyro_Z_RAW = (uint16_t)(data[4] << 8 | data [5]);
	Gx = Gyro_X_RAW/65.5+ 2.7;
	Gy = Gyro_Y_RAW/65.5 - .86;
	Gz = Gyro_Z_RAW/65.5+ 0.40;

	// Calculate the time with seconds
	elapsedTime = MCAL_SysTicK_GetElapsedTime_SingleShot();
	elapsedTime /= 1000000;
	MCAL_SysTicK_StopTimer();

	gyroAngleX = gyroAngleX + Gx * elapsedTime; // deg/s * s = deg
	gyroAngleY = gyroAngleY + Gy * elapsedTime;

	// Angles
	yaw =  yaw + Gz * elapsedTime;

	// Timer on
	MCAL_SysTicK_StartTimer((uint32_t)(8 * 3125.0));

	roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
	pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
}

uint8_t HAL_MPU6050_PitchVal(void){
	return pitch;
}

uint8_t HAL_MPU6050_YawVal(void){
	return yaw;
}

uint8_t HAL_MPU6050_RollVal(void){
	return roll;
}
