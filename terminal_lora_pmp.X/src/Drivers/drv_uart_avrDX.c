/*
 * drv_uart_spx.c
 *
 *  Created on: 11 jul. 2018
 *      Author: pablo
 * 
 * PLACA BASE: sensor_cloro.
 * 
 * El driver de las uart permite crear las uarts y 2 estructuras tipo 
 * ringbuffer (chars) para c/u.
 * Estos son las interfaces a la capa de FRTOS-IO.
 * Para transmitir se escribe en el ringBuffer de TX y para leer lo recibido
 * se lee del ringBuffer de RX.
 * La transmision / recepcion se hace por interrupcion. Estas ISR son 
 * provistas por el driver
 * Cada placa tiene diferente asignacion de puertos por lo tanto hay
 * que modificar el driver a c/placa.
 * 
 * 
 */

#include "drv_uart_avrDX.h"

//------------------------------------------------------------------------------
// USART3: Terminal 
//------------------------------------------------------------------------------
void drv_uart3_init(uint32_t baudrate )
{
    
    PORTB.DIR &= ~PIN1_bm;
    PORTB.DIR |= PIN0_bm;
    USART3.BAUD = (uint16_t)USART_SET_BAUD_RATE(baudrate);     
    USART3.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc;
    
    // Habilito el TX y el RX
    USART3.CTRLB |= USART_TXEN_bm;
    USART3.CTRLB |= USART_RXEN_bm;
    
    // Habilito las interrupciones por RX
    USART3.CTRLA |= USART_RXCIE_bm;
    
    // Las transmisiones son por poleo no INT.
    
    // RingBuffer de transmision
    TXRB_uart3.buff = uart3_txBuffer;
	TXRB_uart3.head = 0;	// start
	TXRB_uart3.tail = 0;	// end
	TXRB_uart3.count = 0;
	TXRB_uart3.length = UART3_TXSIZE;
    
    // RingBuffer de recepcion
    RXRB_uart3.buff = uart3_rxBuffer;
	RXRB_uart3.head = 0;	// start
	RXRB_uart3.tail = 0;	// end
	RXRB_uart3.count = 0;
	RXRB_uart3.length = UART3_RXSIZE;
}
//------------------------------------------------------------------------------
void drv_uart4_init(uint32_t baudrate )
{
    
    PORTE.DIR &= ~PIN1_bm;
    PORTE.DIR |= PIN0_bm;
    USART4.BAUD = (uint16_t)USART_SET_BAUD_RATE(baudrate);     
    USART4.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc;
    
    // Habilito el TX y el RX
    USART4.CTRLB |= USART_TXEN_bm;
    USART4.CTRLB |= USART_RXEN_bm;
    
    // Habilito las interrupciones por RX
    USART4.CTRLA |= USART_RXCIE_bm;
    
    // Las transmisiones son por poleo no INT.
    
    // RingBuffer de transmision
    TXRB_uart4.buff = uart4_txBuffer;
	TXRB_uart4.head = 0;	// start
	TXRB_uart4.tail = 0;	// end
	TXRB_uart4.count = 0;
	TXRB_uart4.length = UART4_TXSIZE;
    
    // RingBuffer de recepcion
    RXRB_uart4.buff = uart4_rxBuffer;
	RXRB_uart4.head = 0;	// start
	RXRB_uart4.tail = 0;	// end
	RXRB_uart4.count = 0;
	RXRB_uart4.length = UART4_RXSIZE;
}
//------------------------------------------------------------------------------
/*
ISR(USART4_DRE_vect)
{
    // ISR de transmisión de la UART4 ( TERM )
    
char cChar = ' ';
int8_t res = false;

	res = rBchar_PopFromISR( &TXRB_uart4, (char *)&cChar );

	if( res == true ) {
		// Send the next character queued for Tx
		USART4.TXDATAL = cChar;
	} else {
		// Queue empty, nothing to send.Apago la interrupcion
        USART4.CTRLB &= ~USART_TXEN_bm;
	}
}
 */
//-----------------------------------------------------------------------------
ISR(USART3_RXC_vect)
{
    // Driver ISR: Cuando se genera la interrupcion por RXIE, lee el dato
    // y lo pone en la cola (ringBuffer.)
char cChar = ' ';

	cChar = USART3.RXDATAL;
 	rBchar_PokeFromISR( &RXRB_uart3, &cChar );
}
//------------------------------------------------------------------------------
ISR(USART4_RXC_vect)
{
    // Driver ISR: Cuando se genera la interrupcion por RXIE, lee el dato
    // y lo pone en la cola (ringBuffer.)
char cChar = ' ';

	cChar = USART4.RXDATAL;
 	rBchar_PokeFromISR( &RXRB_uart4, &cChar );
}
//------------------------------------------------------------------------------
