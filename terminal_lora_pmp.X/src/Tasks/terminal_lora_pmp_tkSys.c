/*
 * File:   tkSys.c
 * Author: pablo
 *
 * 
 */

#include "terminal_lora_pmp.h"
#include "lora.h"
#include "stdlib.h"
#include "nvm.h"

typedef enum { INIT_ST=0, LISTEN_ST, TXMIT_ST, ERROR_ST } states_t;

states_t lora_state;

void pv_INIT_state(void);
void pv_LISTEN_state(void);
void pv_ERROR_state(void);
//------------------------------------------------------------------------------
void tkSys(void * pvParameters)
{

	/*
     * Se encarga de leer la UART del lora y dejarlo en un buffer.
     */

( void ) pvParameters;
uint16_t seed;

	vTaskDelay( ( TickType_t)( 2000 / portTICK_PERIOD_MS ) );
   
	xprintf( "Starting tkSys..\r\n" );
         
    lora_state = INIT_ST;
    seed = nvm_read_signature_sum();
    xprintf_P(PSTR("Seed=%d\r\n"), seed );
    srand(seed);
    
	// loop
	for( ;; )
	{
        vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
        
        switch (lora_state) {
            case INIT_ST:
                pv_INIT_state();
                break;
            case LISTEN_ST:
                pv_LISTEN_state();
                break;
            case TXMIT_ST:
                pv_TXMIT_state();
                break;
            case ERROR_ST:
                pv_ERROR_state();
                break;
            default:
                xprintf_P(PSTR("ERROR: tkSYS state %d\r\n"), lora_state);
                lora_state = INIT_ST;
                break;
        }
        
	}   
}
//------------------------------------------------------------------------------
void pv_ERROR_state(void)
{
    /*
     * Reseteo el modulo lora
     */
    
    xprintf_P(PSTR("DEBUG: IN Error_state\r\n"));

    lora_reset_off();
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    lora_reset_on();
    
    lora_state = INIT_ST;
    xprintf_P(PSTR("DEBUG: OUT Error_state\r\n"));
    return;
}
//------------------------------------------------------------------------------
void pv_TXMIT_state(void)
{
    /* Suelto el semaforo y espera 100ms  con el semaforo liberado. 
     * Si alquien tiene algo para transmitir lo toma y transmite.
     */ 
    
    //xprintf_P(PSTR("DEBUG: IN Txmit_state\r\n"));
    
    xSemaphoreGive( sem_LORA );
    vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
    lora_state = LISTEN_ST;
    
    //xprintf_P(PSTR("DEBUG: OUT Txmit_state\r\n"));
    
}
//------------------------------------------------------------------------------
void pv_LISTEN_state(void)
{
    /*
     * Pasamos el modulo a modo escucha y monitoreamos.
     * Tomo el semaforo y lo mantengo todo lo que dura el listen slot.
     * La duracion es aleatoria entre 15s y 25s
     * Sale porque expira el rx y genera un radio_err
     * 
     */
        
TickType_t slot_time;
float rs;

    
    // Calculo la duracion ( aleatoria ) del slot.
    // Genero un numero aleatorio entre 0 y 6000 
    rs = (6000.0 * rand() ) / RAND_MAX;
    // Calculo el tiempo de duracion del slot que va entre 17000 y 23000 ms
    slot_time = 17000 + (int)rs;
    
    xprintf_P(PSTR("DEBUG: IN Listen_state %d ms\r\n"), slot_time );
    
    // Espero el semaforo para capturar el radio
	while ( xSemaphoreTake( sem_LORA, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 10 ) );
    
    // Paso a modo Listen
    lora_flush_TxBuffer();
    lora_flush_RxBuffer();
    lora_responses_clear();
    sprintf(lora_tx_data.buffer, "radio rx %d", slot_time );
    //strncpy(lora_tx_data.buffer, "radio rx 30000", LORA_TX_BUFFER_SIZE);
    //xfprintf( fdTERM, "[%s]\r\n", &lora_tx_data.buffer[0] );
    xfprintf( fdLORA, "%s\r\n", &lora_tx_data.buffer[0] );
    vTaskDelay( ( TickType_t)( 50 / portTICK_PERIOD_MS ) );
    
    // Monitoreo respuestas
    while(1) {
        // Respuesta ok al comando: no hago nada
        if ( radio_responses.rsp_ok ) {
            ;
        }
        // Error: debo dar de nuevo el comando
        if ( radio_responses.rsp_invalid_params ) {
            break;
        }
        // Error: debo dar de nuevo el comando. Espero 5s.
        if ( radio_responses.rsp_busy ) {
            vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
            break;
        }
            
        // Error del transceiver: Puede ser porque expiro el timeout
        if ( radio_responses.rsp_radio_err ) {
            //vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
            break;
        }
            
        // OK. Recibi un mensaje del remoto. Salgo a procesarlo
        if ( radio_responses.rsp_radio_rx ) {
            lora_print_RxBuffer();
            lora_decode_msg();
            break;      
        } 
            
        vTaskDelay( ( TickType_t)( 50 / portTICK_PERIOD_MS ) );
       
    }
    
    xprintf_P(PSTR("DEBUG: OUT Listen_state\r\n"));  
    lora_state = TXMIT_ST;
    return;

}
//------------------------------------------------------------------------------
void pv_INIT_state(void)
{
    /*
     * Inicializamos los parametros del LORA.
     * No controlo las respuestas !!!
     */
    
    xprintf_P(PSTR("DEBUG: IN Init_state\r\n"));

    // Apagamos el lorawan
    lora_flush_TxBuffer();
    strncpy(lora_tx_data.buffer, "mac pause", LORA_TX_BUFFER_SIZE);
    xfprintf( fdTERM, "[%s]\r\n", &lora_tx_data.buffer[0] );
    xfprintf( fdLORA, "%s\r\n", &lora_tx_data.buffer[0] );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    //
    // Modulamos en fsk
    lora_flush_TxBuffer();
    strncpy(lora_tx_data.buffer, "radio set mod fsk", LORA_TX_BUFFER_SIZE);
    xfprintf( fdTERM, "[%s]\r\n", &lora_tx_data.buffer[0] );
    xfprintf( fdLORA, "%s\r\n", &lora_tx_data.buffer[0] );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    //
    // El tiempo de monitoreo es de 1 min.
    lora_flush_TxBuffer();
    strncpy(lora_tx_data.buffer, "radio set wdt 60000", LORA_TX_BUFFER_SIZE);
    xfprintf( fdTERM, "[%s]\r\n", &lora_tx_data.buffer[0] );
    xfprintf( fdLORA, "%s\r\n", &lora_tx_data.buffer[0] );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    
    lora_state = LISTEN_ST;
    xprintf_P(PSTR("DEBUG: OUT Init_state\r\n"));
    return;

}
//------------------------------------------------------------------------------

