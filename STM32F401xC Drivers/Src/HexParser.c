/*
 * HexParser.c
 *
 * Created on: Sep 24, 2025
 * Author: Ahmed Ezzat
 */

#include "HexParser.h"


#define G_HIGH_ADDRESS	0x08000000

static uint8_t HexParser_ASCCI_To_HEX(uint8_t ASCCI){
	uint8_t Value = 0;

	if(ASCCI >= '0' &&  ASCCI <='9'){
		Value =  ASCCI - '0';
	}
	else{
		Value = ASCCI - 55;
	}

	return Value;
}

void APP_HexParser_ParseRecord(const sint8_t  *Record){
	uint8_t Var = HexParser_ASCCI_To_HEX(Record[8]);

	switch(Var){
		case 0:
			APP_HexParser_ParseData(Record); break;

		// End of File
		case 1:  break;

		// Set High Address
		case 4:  break;

		default: break;
	}
}

void APP_HexParser_ParseData(const sint8_t *Record){

	// Set Character Count
	uint8_t CC_high = HexParser_ASCCI_To_HEX(Record[1]);
	uint8_t CC_low  = HexParser_ASCCI_To_HEX(Record[2]);
	uint8_t CC      = (CC_high<<4) | CC_low;

	// Low Part Address
	uint8_t Address0 = HexParser_ASCCI_To_HEX(Record[3]);
	uint8_t Address1 = HexParser_ASCCI_To_HEX(Record[4]);
	uint8_t Address2 = HexParser_ASCCI_To_HEX(Record[5]);
	uint8_t Address3 = HexParser_ASCCI_To_HEX(Record[6]);

	// 0b 0000 0000 Address2 Address3
	uint16_t LowAddress =  (Address0<<12) | (Address1<<8) | (Address2<<4) | (Address3);

	uint32_t Address = G_HIGH_ADDRESS | LowAddress;

	uint16_t Data[10] = {0};

	for(uint8_t i = 0; i < (CC/2); i++){
		uint8_t digit0 = HexParser_ASCCI_To_HEX(Record[9  + (4*i)]);
		uint8_t digit1 = HexParser_ASCCI_To_HEX(Record[10 + (4*i)]);
		uint8_t digit2 = HexParser_ASCCI_To_HEX(Record[11 + (4*i)]);
		uint8_t digit3 = HexParser_ASCCI_To_HEX(Record[12 + (4*i)]);

		// Data[i] = (digit0<<12) | (digit1<<8) | (digit2<<4) | (digit3); //
		Data[i] = (digit0<<4) | (digit1) | (digit2<<12) | (digit3<<8); //
	}

	MCAL_FMI_ProgramFlash(Address, Data, (CC/2));
}


