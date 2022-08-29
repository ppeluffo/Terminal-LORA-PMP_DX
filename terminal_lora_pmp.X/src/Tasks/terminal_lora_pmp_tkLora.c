/*
 * File:   tkLora.c
 * Author: pablo
 *
 * 
 */

#include "terminal_lora_pmp.h"
#include "lora.h"
#include "usart.h"

void LORA_process( uint8_t c);

//------------------------------------------------------------------------------
void tkLora(void * pvParameters)
{

	/*
     * Se encarga de leer la UART del lora y dejarlo en un buffer.
     */

( void ) pvParameters;
uint8_t c = 0;

	vTaskDelay( ( TickType_t)( 300 / portTICK_PERIOD_MS ) );
   
	xprintf( "Starting tkLora..\r\n" );
    lora_reset_on();
    lora_flush_RxBuffer();
    
	// loop
	for( ;; )
	{
		//c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 50ms. lo que genera la espera.
		while ( frtos_read( fdLORA, (char *)&c, 1 ) == 1 ) {
            LORA_process(c);
        }
        vTaskDelay( ( TickType_t)( 1 / portTICK_PERIOD_MS ) );
        
	}   
}
//------------------------------------------------------------------------------
void LORA_process( uint8_t c)
{
    /*
     * El proceso de los datos recibidos por el puerto lora consiste en encolarlos
     * y mandarlos por el puerto de la terminal.
     * 
     */
    
    if ( c=='\r') {
        
        // Respuesta del transceiver
        if ( strstr( lora_rx_data.buffer, "radio_rx") != NULL ) {
            radio_responses.rsp_radio_rx = true;
        } else if ( strstr( lora_rx_data.buffer, "radio_tx_ok") != NULL ) {      
            radio_responses.rsp_radio_tx_ok = true;
        } else if ( strstr( lora_rx_data.buffer, "radio_err") != NULL ) {          
            radio_responses.rsp_radio_err = true;
        }
        
        // Respuesta al comando
        if ( strstr( lora_rx_data.buffer, "ok") != NULL ) {
            radio_responses.rsp_ok = true;
        } else if ( strstr( lora_rx_data.buffer, "invalid_params") != NULL ) {      
            radio_responses.rsp_invalid_params = true;
        } else if ( strstr( lora_rx_data.buffer, "busy") != NULL ) {          
            radio_responses.rsp_busy = true;
        }
        
    } 
    
    lora_push_RxBuffer(c);
    xputChar(c);
    return;
 
}
//------------------------------------------------------------------------------