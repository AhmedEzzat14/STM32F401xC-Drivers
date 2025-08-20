/*
 * SevenSegment_driver.c
 *
 * Created on: Aug 18, 2025
 * Author: Ahmed Ezzat
 */

//------------------------------
// Includes
//------------------------------
#include "SevenSegment_driver.h"

//=========================================================

//-------------------------
// Macros Data Types
//-------------------------
// a,b,c,d,e,f,g = bit0..bit6
static const uint8_t SevenSeg_LUT[10] = {
		0b00111111,//0
		0b00000110,//1
		0b01011011,//2
		0b01001111,//3
		0b01100110,//4
		0b01101101,//5
		0b01111101,//6
		0b00000111,//7
		0b01111111,//8
		0b01101111//9
};

//=========================================================

/*
 * =========================================================
 * 		 APIs Supported by "HAL Seven Segment DRIVER"
 * =========================================================
 */

void HAL_SEVEN_SEG_WriteNumber_CommCathode(uint16_t Number, BCD_Status_t bcd_status){
	if(enable == bcd_status){
        for(uint8_t i=0; i<4; i++){
             if(Number & (1<<i)){
                 MCAL_GPIO_WritePin(GPIOA, (1<<i), GPIO_PIN_SET);
             }
             else{
                 MCAL_GPIO_WritePin(GPIOA, (1<<i), GPIO_PIN_RST);
             }
         }
	}
	else{
		uint8_t pattern = SevenSeg_LUT[Number];
		for(uint8_t i = 0; i < 7; i++){
			if((pattern>>i) & 1){
				MCAL_GPIO_WritePin(GPIOA, (1<<i), GPIO_PIN_SET);
			}
			else{
				MCAL_GPIO_WritePin(GPIOA, (1<<i), GPIO_PIN_RST);
			}
		}
	}
}

void HAL_SEVEN_SEG_WriteNumber_CommAnode(uint16_t Number, BCD_Status_t bcd_status){
    // Check if the number is higher than 9
	if(Number > 9){
		for(uint8_t i = 0; i < 7; i++){
			MCAL_GPIO_WritePin(GPIOA, (1<<i), GPIO_PIN_SET);
		}
	}

	if(enable == bcd_status){
        for(uint8_t i=0; i<4; i++){
             if(Number & (1<<i)){
                 MCAL_GPIO_WritePin(GPIOA, (1<<i), GPIO_PIN_SET);
             }
             else{
                 MCAL_GPIO_WritePin(GPIOA, (1<<i), GPIO_PIN_RST);
             }
         }
	}
	else{
		uint8_t pattern = ~SevenSeg_LUT[Number];
		for(uint8_t i = 0; i < 7; i++){
				MCAL_GPIO_WritePin(GPIOA, i, GET_BIT(pattern, i));
		}

	}
}
