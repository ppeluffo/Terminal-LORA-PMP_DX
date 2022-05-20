/*
 * File:   tkLora.c
 * Author: pablo
 *
 * 
 */

#include "terminal_lora_pmp.h"
//#include "lora.h"

//------------------------------------------------------------------------------
void tkRxLora(void * pvParameters)
{

	/*
     * Se encarga de leer la UART del lora y dejarlo en un buffer.
     */

( void ) pvParameters;
//uint8_t c = 0;

	vTaskDelay( ( TickType_t)( 300 / portTICK_PERIOD_MS ) );
    LORA_init();
   
    //lora_reset_on();
	xprintf( "Starting tkRxLora..\r\n" );
            
	// loop
	for( ;; )
	{
        kick_wdt(LRA_WDG_bp);
		//c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 50ms. lo que genera la espera.
		//while ( frtos_read( fdLORA, (char *)&c, 1 ) == 1 ) {
           //loraRx_process(c);
           vTaskDelay( ( TickType_t)( 1 / portTICK_PERIOD_MS ) );
        //}
        
	}   
}
//------------------------------------------------------------------------------
