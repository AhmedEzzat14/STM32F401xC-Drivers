/*
 * IR_driver.c
 *
 * Created on: Aug 31, 2025
 * Author: Ahmed Ezzat
 */

//------------------------------
// Includes
//------------------------------
#include "IR_driver.h"

//============================================================

/*
 * =======================================================
 * 		    Generic Variables
 * =======================================================
 */
uint8_t G_value;
uint8_t G_StartFlag;
uint8_t G_Counter;
uint32_t IR_array[50]={0};


uint8_t test = 0;
//============================================================

/*
 * =======================================================
 * 		    Helper Functions
 * =======================================================
 */
uint8_t get_value(void);

//============================================================

/*
 * ==================================================
 * 	    APIs Supported by "IR DRIVER"
 * ==================================================
 */

void HAL_IR_GetTime(void){
    if(G_StartFlag == 0){
	// For Start Bit, should be used one time only
	G_StartFlag = 1;
	test = 9;

	test = MCAL_SysTicK_SetInterval_Single(15, HAL_IR_DecodeBits);

    }
    else{
	IR_array[G_Counter++] = (MCAL_SysTicK_GetElapsedTime_SingleShot() / 3.125);
	MCAL_SysTicK_SetInterval_Single(4, HAL_IR_DecodeBits);

	test = 2;
    }
}

void HAL_IR_DecodeBits(void){
    G_value = 0;

    uint8_t i = 0;

    for(i = 0; i < 8; i++){
	if ((IR_array[17+i] >= 400) && (IR_array[17+i] < 800)) {
	    G_value &= ~(1 << i);   // "0"
	}
	else if ((IR_array[17+i] >= 1500) && (IR_array[17+i] < 2000)) {
	    G_value |= (1 << i);    // "1"
	}
    }

    // clear flag, counter and array
    G_Counter = 0;
    G_StartFlag = 0;

    for(uint8_t i=0;i<50;i++){
	IR_array[i] = 0;
    }

    test = 3;

    //return G_value;
}

uint8_t get_value(void){
    return G_value;
}
