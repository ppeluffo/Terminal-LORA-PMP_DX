/*
 * l_i2c.c
 *
 *  Created on: 26 de mar. de 2018
 *      Author: pablo
 */

#include "i2c.h"

char buffer[10] = { 0 };

//------------------------------------------------------------------------------
int16_t I2C_write ( uint8_t devAddress, 
        uint16_t dataAddress, 
        uint8_t dataAddress_length, 
        char *data, 
        uint8_t data_length )
{
	

int16_t xReturn = 0U;
uint8_t i2c_error_code = 0;
   
    frtos_ioctl(fdI2C,ioctl_OBTAIN_BUS_SEMPH, NULL);

	// 1) Indicamos el periferico i2c en el cual queremos escribir ( variable de 8 bits !!! )
	frtos_ioctl(fdI2C,ioctl_I2C_SET_DEVADDRESS, &devAddress );

	// 2) Indicamos al direccion interna del chip donde comenzar a escribir
	frtos_ioctl(fdI2C,ioctl_I2C_SET_DATAADDRESS, &dataAddress );
	frtos_ioctl(fdI2C,ioctl_I2C_SET_DATAADDRESSLENGTH, &dataAddress_length );

	// 3) Por ultimo escribimos. No controlo fronteras.
	xReturn = frtos_write(fdI2C, data, data_length);

	// 4) Controlo errores
	i2c_error_code = frtos_ioctl(fdI2C, ioctl_I2C_GET_LAST_ERROR, NULL );
	if (i2c_error_code != I2C_OK ) {
		xReturn = -1;
	}
    
    frtos_ioctl(fdI2C,ioctl_RELEASE_BUS_SEMPH, NULL);
    
	return(xReturn);
}
//------------------------------------------------------------------------------
int16_t I2C_read  ( uint8_t devAddress, 
        uint16_t dataAddress, 
        uint8_t dataAddress_length, 
        char *data, 
        uint8_t data_length )
{

	// Implementa solo la parte de lectura del ciclo.
	// La primera parte que es solo escribir la direccion de donde leer la hacemos
	// con I2C_write_R1. ( Dummy Write )

int16_t xReturn = 0U;
uint8_t i2c_error_code = 0;

    frtos_ioctl( fdI2C,ioctl_OBTAIN_BUS_SEMPH, NULL);

	// 1) Indicamos el periferico i2c en el cual queremos escribir ( variable de 8 bits !!! )
	frtos_ioctl(fdI2C,ioctl_I2C_SET_DEVADDRESS, &devAddress );

	// 2) Indicamos al direccion interna del chip donde comenzar a escribir
	frtos_ioctl(fdI2C,ioctl_I2C_SET_DATAADDRESS, &dataAddress );
	frtos_ioctl(fdI2C,ioctl_I2C_SET_DATAADDRESSLENGTH, &dataAddress_length );

	// 3) Leemos. No controlo fronteras.
	xReturn = frtos_read(fdI2C, data, data_length);

	// 4) Controlo errores.
	i2c_error_code = frtos_ioctl(fdI2C, ioctl_I2C_GET_LAST_ERROR, NULL );
	if (i2c_error_code != I2C_OK ) {
		xReturn = -1;
	}

	if (xReturn != data_length ) {
		xReturn = -1;
	}

    frtos_ioctl(fdI2C,ioctl_RELEASE_BUS_SEMPH, NULL);
    
	return(xReturn);

}
//------------------------------------------------------------------------------
