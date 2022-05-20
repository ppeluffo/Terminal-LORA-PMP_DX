/*
 * l_i2c.h
 *
 *  Created on: 26 de mar. de 2018
 *      Author: pablo
 */

#ifndef SPX_LIBS_L_I2C_H_
#define SPX_LIBS_L_I2C_H_

#include "stdint.h"
#include "frtos-io.h"
#include "xprintf.h"

#define BUSADDR_EEPROM_M2402	0xA0
#define BUSADDR_RTC_M79410		0xDE
#define BUSADDR_MCP23018		0x4E
#define BUSADDR_INA_A			0x80
#define BUSADDR_INA_B			0x82
#define BUSADDR_INA_C			0x86
#define BUSADDR_BPS120			0x50
#define BUSADDR_ADT7410			0x90


int16_t I2C_read  ( uint8_t devAddress, uint16_t dataAddress, uint8_t dataAddress_length, char *data, uint8_t data_length );
int16_t I2C_write ( uint8_t devAddress, uint16_t dataAddress, uint8_t dataAddress_length, char *data, uint8_t data_length );

#endif /* SPX_LIBS_L_I2C_H_ */
