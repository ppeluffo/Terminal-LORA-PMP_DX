
#include "lora.h"


// -----------------------------------------------------------------------------
void LORA_init(void)
{
    LORA_RTS_CONFIG();
    LORA_CTS_CONFIG();
    LORA_RESET_CONFIG();
    
    //lBchar_CreateStatic(&lora_rx_sdata, (char *)&lora_rx_buffer, LORA_RX_BUFFER_SIZE );
}
// -----------------------------------------------------------------------------
void lora_flush_RxBuffer(void)
{
    //lBchar_Flush(&lora_rx_sdata);
    //memset( &lora_rx_buffer, '\0', LORA_RX_BUFFER_SIZE);
    //lora_rx_sdata.ptr = 0;
    memset( lora_rx_data.buffer, '\0', LORA_RX_BUFFER_SIZE );
    lora_rx_data.ptr = 0;
    
}
// -----------------------------------------------------------------------------
void lora_flash_led(void)
{
    xprintf( "Lora test led.\r\n");

    xfprintf( fdLORA, "sys set pindig GPIO5 1\r\n");
    vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
    xfprintf( fdLORA, "sys set pindig GPIO5 0\r\n");
}
// -----------------------------------------------------------------------------
void lora_print_RxBuffer(void)
{
    // xprintf( "[%s]\r\n", lora_rx_sdata.buff  );
    xprintf( "RXBUFF [%s]\r\n", lora_rx_data.buffer  );
}
//------------------------------------------------------------------------------
void lora_push_RxBuffer( uint8_t c)
{
    //lBchar_Poke(&lora_rx_sdata,  (char *)&c);
    if ( lora_rx_data.ptr < LORA_RX_BUFFER_SIZE ) {
        lora_rx_data.buffer[lora_rx_data.ptr++] = c;
        
    }
}
//------------------------------------------------------------------------------
void lora_print_RxBuffer_stats(void)
{
    
//uint8_t i;
    
 /*    xprintf("lRxBuff_ptr [%d]\r\n", lora_rx_sdata.ptr );
     xprintf("lRxBuff_size[%d]\r\n", lora_rx_sdata.size );
     xprintf("lRxBuff_data[%s]\r\n", lora_rx_sdata.buff );
     xprintf("DEBUG[");
     for (i=0; i < lora_rx_sdata.ptr; i++) {
        xprintf("%c", lora_rx_sdata.buff[i] );
     }
     xprintf("]\r\n");
*/
     xprintf("ptr [%d]\r\n", lora_rx_data.ptr );
     xprintf("[%s]\r\n", lora_rx_data.buffer );
     /*
     xprintf("DEBUG[");
     for (i=0; i < lora_rx_data.ptr; i++) {
        xprintf("%c", lora_rx_data.rx_buffer[i] );
     }
     xprintf("]\r\n");
     
     xprintf( "[%s]\r\n", lora_rx_data.rx_buffer );
     //xprintf( "lRxBuff_data[%s]\r\n", lora_rx_data.rx_buffer );
      */
}
//------------------------------------------------------------------------------
void lora_flush_TxBuffer(void)
{
    memset( lora_tx_data.buffer, '\0', LORA_TX_BUFFER_SIZE );
    lora_tx_data.ptr = 0;
}
//------------------------------------------------------------------------------
void lora_responses_clear(void)
{
    radio_responses.rsp_ok = false;
    radio_responses.rsp_invalid_params = false;
    radio_responses.rsp_busy = false;
    radio_responses.rsp_radio_rx = false;
    radio_responses.rsp_radio_tx_ok = false;
    radio_responses.rsp_radio_err = false;
    
}
//------------------------------------------------------------------------------
void lora_responses_print(void)
{
   xprintf_P(PSTR("ok:%d\r\n"),radio_responses.rsp_ok);
   xprintf_P(PSTR("invalid_params:%d\r\n"),radio_responses.rsp_invalid_params);
   xprintf_P(PSTR("busy:%d\r\n"),radio_responses.rsp_busy);
   xprintf_P(PSTR("radio_rx:%d\r\n"),radio_responses.rsp_radio_rx);
   xprintf_P(PSTR("radio_tx_ok:%d\r\n"),radio_responses.rsp_radio_tx_ok);
   xprintf_P(PSTR("radio_err:%d\r\n"),radio_responses.rsp_radio_err);
}
//------------------------------------------------------------------------------
void lora_cmd(char **argv )
{
    
uint8_t i;
uint8_t j;

    //xprintf_P(PSTR("LORA CMD\r\n"));
    j=0;
    
    lora_flush_TxBuffer();
    //lora_flush_RxBuffer();
    
    // Genero el comando a mandar al modulo lora.
    // Arranco de 1 porque no quiero el 'lora '
    // El argumento 1 lo paso a minuscula porque el parseo lo cambio
    if ( argv[1] != NULL ) {
         argv[1] = strlwr(argv[1]);
    }
    
    for (i=1; i<16; i++) {
        if ( argv[i] != NULL ) {
            //xprintf_P(PSTR("arg#%02d: %s\r\n"), i, argv[i]);
            j += snprintf( &lora_tx_data.buffer[j], ( LORA_TX_BUFFER_SIZE - j - 1), "%s ", argv[i] );
        }
    }
    
    // Al final quedo un espacion en blanco que debo eliminarlo  
    lora_tx_data.buffer[ strlen(lora_tx_data.buffer) - 1] = '\0'; 
    
    xfprintf( fdTERM, "[%s]\r\n", &lora_tx_data.buffer[0] );
    xfprintf( fdLORA, "%s\r\n", &lora_tx_data.buffer[0] );
    vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
    
}
//------------------------------------------------------------------------------
bool lora_send_msg_in_ascii(char *msg)
{
    /*
     * Convierte en mensaje en ASCII sobre el lora_tx_buffer y luego 
     * manda el comando.
     * La respuesta al comando puede ser: { ok, invalid_param, busy }
     * La respuesta despues de transmitirlo puede ser: {radio_tx_ok, radio_err }
     * 
     * En caso que la respuesta no sea 'radio_tx_ok', reintentamos 3 veces.
     * 
     * Al finalizar volvemos al modo recepcion.
     */
  
uint8_t i = 0;
char *p;
char c;
uint8_t timeout;
bool retS = false;

    lora_flush_TxBuffer(); 
    
    // Armo el frame.
    i += snprintf( &lora_tx_data.buffer[i], LORA_TX_BUFFER_SIZE, "radio tx ");   
    p=msg;
    
    while (*p) {
        c=*p++;
        i += snprintf( &lora_tx_data.buffer[i], LORA_TX_BUFFER_SIZE, "%x",c); 
        //xprintf_P(PSTR("%c=%x\r\n"), c, c);
    }
    
    // Espero el semaforo para capturar el radio
	while ( xSemaphoreTake( sem_LORA, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 10 ) );
    
    lora_flush_RxBuffer();
    lora_responses_clear();
    xprintf_P(PSTR("TX=[%s]\r\n"), lora_tx_data.buffer);
    xfprintf( fdLORA, "%s\r\n", &lora_tx_data.buffer[0] );
    
    // Espero alguna respuesta hasta 5 secs.
    timeout = 0;
    while( timeout++ < 50) {
        // OK. Envio el mensaje correctamente
        if ( radio_responses.rsp_radio_tx_ok ) {
            retS = true;
            break;
        }    
        vTaskDelay( ( TickType_t)( 50 / portTICK_PERIOD_MS ) );
    }
    
    xSemaphoreGive( sem_LORA );
    return (retS);
}
//------------------------------------------------------------------------------
void lora_decode_msg(void)
{
    /*
     Decodifica el mensaje que hay en el rxBuffer.
     */
    
uint8_t *p = NULL;
uint8_t c0,c1;
uint8_t d0,d1;
uint8_t dec;


    // Elimino todo hasta el 'radio_rx '
    p = (uint8_t *)strstr(lora_rx_data.buffer, "radio_rx" );
    if (p == NULL)
        return;

    // Elimino posibles caracteres al inicio
    while (*p != ' ')
        p++;

    // Elimino los caracteres en blanco que quedan
    while (*p == ' ')
        p++;
    
    xprintf_P(PSTR("RXMSG:[%s]\r\n"),p);

    while (*p != '\r') {
        c0 = *p++;
        c1 = *p++;
        
        // C0 es numerico ?
        d0 = 0;
        if ( c0 < 58 ) {
            d0 = c0 - '0';
        } else if ( c0 < 71) {
            d0 = c0 - 'A' + 10;
        }
        
        // C1 es numerico ?
        d1 = 0;
        if ( c1 < 58 ) {
            d1 = c1 - '0';
        } else if ( c1 < 71) {
            d1 = c1 - 'A' + 10;
        }
        
        dec = d0*16 + d1;
        
        xprintf_P(PSTR("CHR: %d, %c\r\n"),dec, dec);
    }
    
}
//------------------------------------------------------------------------------
// DEPRECATED

void lora_radio_init(void)
{
    /*
     * Comando para configurar el modulo para P2P
     */
    
    lora_flush_TxBuffer();
    strncpy(lora_tx_data.buffer, "mac pause", LORA_TX_BUFFER_SIZE);
    xfprintf( fdTERM, "[%s]\r\n", &lora_tx_data.buffer[0] );
    xfprintf( fdLORA, "%s\r\n", &lora_tx_data.buffer[0] );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    //
    lora_flush_TxBuffer();
    strncpy(lora_tx_data.buffer, "radio set mod fsk", LORA_TX_BUFFER_SIZE);
    xfprintf( fdTERM, "[%s]\r\n", &lora_tx_data.buffer[0] );
    xfprintf( fdLORA, "%s\r\n", &lora_tx_data.buffer[0] );
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    //
    
}
//------------------------------------------------------------------------------
