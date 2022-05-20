/*
 * drv_uart_spx.h
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#ifndef SRC_SPX_DRIVERS_DRV_UART_SPX_H_
#define SRC_SPX_DRIVERS_DRV_UART_SPX_H_

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "FreeRTOS.h"

#include "ringBuffer.h"
//#include <xc.h>


#ifndef F_CPU
#define F_CPU 24000000
#endif

#define USART_SET_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5);

//-----------------------------------------------------------------------
#define UART3_TXSIZE	8	// trasmito por poleo. Si uso interrupcion lo subo a 128
uint8_t uart3_txBuffer[UART3_TXSIZE];

#define UART3_RXSIZE	64	// 
uint8_t uart3_rxBuffer[UART3_RXSIZE];

#define UART4_TXSIZE	8	// trasmito por poleo. Si uso interrupcion lo subo a 128
uint8_t uart4_txBuffer[UART4_TXSIZE];

#define UART4_RXSIZE	64	// 
uint8_t uart4_rxBuffer[UART4_RXSIZE];

rBchar_s TXRB_uart3, RXRB_uart3;
rBchar_s TXRB_uart4, RXRB_uart4;

void drv_uart3_init(uint32_t baudrate );
void drv_uart4_init(uint32_t baudrate );

//-----------------------------------------------------------------------




#endif /* SRC_SPX_DRIVERS_DRV_UART_SPX_H_ */
