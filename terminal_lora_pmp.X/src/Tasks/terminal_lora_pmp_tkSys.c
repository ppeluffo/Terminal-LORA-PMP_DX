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
void pv_TXMIT_state(void);
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
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
        
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
    rs = (1.0 * systemConf.lora_rxslot_spread * rand() ) / RAND_MAX;
    // Calculo el tiempo de duracion del slot que va entre 17000 y 23000 ms
    slot_time = systemConf.lora_rxslot_width + (int)rs;
    
    //xprintf_P(PSTR("DEBUG: IN Listen_state %d ms\r\n"), slot_time );
    
    // Espero el semaforo para capturar el radio
	while ( xSemaphoreTake( sem_LORA, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 10 ) );
    
    // Paso a modo Listen
    lBchar_Flush(&lora_tx_sdata);
    lBchar_Flush(&lora_rx_sdata);
    lora_responses_clear();
    
    sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio rx %d", slot_time );
    if (systemConf.debug_lora_comms) {
        xfprintf( fdTERM, "%s\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    }
    xfprintf( fdLORA, "%s\r\n", lBchar_get_buffer(&lora_tx_sdata) );
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
            //vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
            break;
        }
            
        // Error del transceiver: Puede ser porque expiro el timeout
        if ( radio_responses.rsp_radio_err ) {
            break;
        }
            
        // OK. Recibi un mensaje del remoto. Salgo a procesarlo
        if ( radio_responses.rsp_radio_rx ) {
            //lBchar_print(&lora_rx_sdata);
            //lora_print_rxvd_msg();
            lora_decode_msg();
            lora_read_snr();
            lora_send_confirmation();
            break;      
        } 
            
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
       
    }
    
    //xprintf_P(PSTR("DEBUG: OUT Listen_state\r\n"));  
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
    
    //xprintf_P(PSTR("DEBUG: IN Init_state\r\n"));
    xprintf_P(PSTR("LORA: Init...\r\n"));

    //lora_reset_off();
    //vTaskDelay( ( TickType_t)( 2000 / portTICK_PERIOD_MS ) );
    //lora_reset_on();
    
    // Apagamos el lorawan
    lBchar_Flush(&lora_tx_sdata);
    sprintf( lBchar_get_buffer(&lora_tx_sdata), "mac pause" );
    xfprintf( fdTERM, "[%s]\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    xfprintf( fdLORA, "%s\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    //
    // Modulamos en fsk
    lBchar_Flush(&lora_tx_sdata);
    sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set mod fsk" );
    xfprintf( fdTERM, "[%s]\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    xfprintf( fdLORA, "%s\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    //
    // El tiempo de monitoreo es de 1 min.
    lBchar_Flush(&lora_tx_sdata);
    sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set wdt 60000" );
    xfprintf( fdTERM, "[%s]\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    xfprintf( fdLORA, "%s\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    //
    // Potencia de transmision
    lBchar_Flush(&lora_tx_sdata);
    sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set pwr %d", systemConf.lora_pwrOut );
    xfprintf( fdTERM, "[%s]\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    xfprintf( fdLORA, "%s\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    // 
    // Bandwidth
    lBchar_Flush(&lora_tx_sdata);
    sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set bw %d", systemConf.lora_bw );
    xfprintf( fdTERM, "[%s]\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    xfprintf( fdLORA, "%s\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
     // 
    // SpreadFacotor
    lBchar_Flush(&lora_tx_sdata);
    switch( systemConf.lora_spreadFactor ) {
        case  sf7:
            sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set sf sf7");
            break;
        case sf8:
            sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set sf sf8");
            break;
        case sf9:
            sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set sf sf9");
            break;
        case sf10:
            sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set sf sf10");
            break;
        case sf11:
            sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set sf sf11");
            break;
        case sf12:
            sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set sf sf12");
            break;
        default:
            sprintf( lBchar_get_buffer(&lora_tx_sdata), "radio set sf sf12");
            break;
    }
    //
    xfprintf( fdTERM, "[%s]\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    xfprintf( fdLORA, "%s\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    
    lora_state = LISTEN_ST;
    //xprintf_P(PSTR("DEBUG: OUT Init_state\r\n"));
    xprintf_P(PSTR("LORA: Listening...\r\n"));
    return;

}
//------------------------------------------------------------------------------

