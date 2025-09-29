/*
 * HexParser.h
 *
 * Created on: Sep 24, 2025
 * Author: Ahmed Ezzat
 */

#ifndef HEXPARSER_H_
#define HEXPARSER_H_

#include "stm32f401xc.h"
#include "stm32f401xc_FMI_driver.h"

void APP_HexParser_ParseRecord(const sint8_t *Record);
void APP_HexParser_ParseData(const sint8_t *Record);

#endif /* HEXPARSER_H_ */
