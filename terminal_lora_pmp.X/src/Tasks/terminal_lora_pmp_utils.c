/* 
 * File:   frtos20_utils.c
 * Author: pablo
 *
 * Created on 22 de diciembre de 2021, 07:34 AM
 */


#include "terminal_lora_pmp.h"
#include "xprintf.h"
#include "led.h"
#include "dac.h"
#include "nvm.h"
#include "lora.h"

//------------------------------------------------------------------------------
int8_t WDT_init(void);
int8_t CLKCTRL_init(void);

//-----------------------------------------------------------------------------
void system_init()
{
//	mcu_init();

	CLKCTRL_init();
    WDT_init();
    LED_init();
    XPRINTF_init();
    VREF_init();
    DAC_init();
    LORA_init();
    
}
//-----------------------------------------------------------------------------
int8_t WDT_init(void)
{
	/* 8K cycles (8.2s) */
	/* Off */
	ccp_write_io((void *)&(WDT.CTRLA), WDT_PERIOD_8KCLK_gc | WDT_WINDOW_OFF_gc );  
	return 0;
}
//-----------------------------------------------------------------------------
int8_t CLKCTRL_init(void)
{
	// Configuro el clock para 24Mhz
	
	ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA), CLKCTRL_FREQSEL_24M_gc         /* 24 */
	| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable: disabled */
	| 0 << CLKCTRL_RUNSTDBY_bp /* Run standby: disabled */);

	// ccp_write_io((void*)&(CLKCTRL.MCLKCTRLA),CLKCTRL_CLKSEL_OSCHF_gc /* Internal high-frequency oscillator */
	//		 | 0 << CLKCTRL_CLKOUT_bp /* System clock out: disabled */);

	// ccp_write_io((void*)&(CLKCTRL.MCLKLOCK),0 << CLKCTRL_LOCKEN_bp /* lock enable: disabled */);

	return 0;
}
//-----------------------------------------------------------------------------
void reset(void)
{
	/* Issue a Software Reset to initilize the CPU */
	ccp_write_io( (void *)&(RSTCTRL.SWRR), RSTCTRL_SWRST_bm );  
}
//------------------------------------------------------------------------------
void config_default(void)
{
    systemConf.timerPoll = 60;
    systemConf.debug_lora_comms = false;
    
    systemConf.lora_pwrOut = 14;
    systemConf.lora_spreadFactor = sf12;
    systemConf.lora_bw = 125;
    
    systemConf.tipo_dispositivo = tanque;
    
    systemConf.lora_rxslot_width = 7000;
    systemConf.lora_rxslot_spread = 6000;
    
}
//------------------------------------------------------------------------------
bool save_config_in_NVM(void)
{
   
int8_t retVal;
uint8_t cks;

    cks = checksum ( (uint8_t *)&systemConf, ( sizeof(systemConf) - 1));
    systemConf.checksum = cks;
    
    retVal = NVM_EE_write( 0x00, (char *)&systemConf, sizeof(systemConf) );
    if (retVal == -1 )
        return(false);
    
    return(true);
   
}
//------------------------------------------------------------------------------
bool load_config_from_NVM(void)
{

uint8_t rd_cks, calc_cks;
    
    NVM_EE_read( 0x00, (char *)&systemConf, sizeof(systemConf) );
    rd_cks = systemConf.checksum;
    
    calc_cks = checksum ( (uint8_t *)&systemConf, ( sizeof(systemConf) - 1));
    
    if ( calc_cks != rd_cks ) {
		xprintf_P( PSTR("ERROR: Checksum systemConf failed: calc[0x%0x], read[0x%0x]\r\n"), calc_cks, rd_cks );
        
		return(false);
	}
    
    return(true);
}
//------------------------------------------------------------------------------
uint8_t checksum( uint8_t *s, uint16_t size )
{
	/*
	 * Recibe un puntero a una estructura y un tamaño.
	 * Recorre la estructura en forma lineal y calcula el checksum
	 */

uint8_t *p = NULL;
uint8_t cks = 0;
uint16_t i = 0;

	cks = 0;
	p = s;
	for ( i = 0; i < size ; i++) {
		 cks = (cks + (int)(p[i])) % 256;
	}

	return(cks);
}
//------------------------------------------------------------------------------
void kick_wdt( uint8_t bit_pos)
{
    sys_watchdog &= ~ (1 << bit_pos);
    
}
//------------------------------------------------------------------------------
bool config_timerpoll(char *s_timerpoll)
{
    systemConf.timerPoll = atoi(s_timerpoll);
    return (true);
}
//------------------------------------------------------------------------------
bool config_pwrOut(char *s_pwrOut)
{
    
int8_t pwrOut;
    
    pwrOut = atoi(s_pwrOut);
    if ( (pwrOut < 2) || (pwrOut > 20) ) {
        return (false);
    }
    if ( (pwrOut == 18) || (pwrOut == 19) ) {
        return (false);
    }
    systemConf.lora_pwrOut = pwrOut;
    return (true);
    
}
//------------------------------------------------------------------------------
bool config_bandWidth(char *s_bandWidth)
{
    
int16_t bandWidth;
    
    bandWidth = atoi(s_bandWidth);   
    if ( (bandWidth == 125) || (bandWidth == 250) || ( bandWidth == 500)) {
        systemConf.lora_bw = bandWidth;
        return (true);
    }
        
    return (false);
    
}
//------------------------------------------------------------------------------
bool config_spreadFactor(char *s_spreadFactor)
{
    
    if ( strcmp( strupr(s_spreadFactor),"SF7") == 0 ) {
        systemConf.lora_spreadFactor = sf7;
        return(true);
    } else if ( strcmp( strupr(s_spreadFactor),"SF8") == 0 ) {
        systemConf.lora_spreadFactor = sf8;
        return(true);
    } else if ( strcmp( strupr(s_spreadFactor),"SF9") == 0 ) {
        systemConf.lora_spreadFactor = sf9;
        return(true);
    } else if ( strcmp( strupr(s_spreadFactor),"SF10") == 0 ) {
        systemConf.lora_spreadFactor = sf10;
        return(true);
    } else if ( strcmp( strupr(s_spreadFactor),"SF11") == 0 ) {
        systemConf.lora_spreadFactor = sf11;
        return(true);
    } else if ( strcmp( strupr(s_spreadFactor),"SF12") == 0 ) {
        systemConf.lora_spreadFactor = sf12;
        return(true);
    }

    return (false);
    
}
//------------------------------------------------------------------------------
bool config_tipoDispositivo(char *s_tipoDispositivo)
{
    
    if ( strcmp( strupr(s_tipoDispositivo),"TANQUE") == 0 ) {
        systemConf.tipo_dispositivo = tanque;
        return(true);
    } else if ( strcmp( strupr(s_tipoDispositivo),"PERFORACION") == 0 ) {
        systemConf.tipo_dispositivo = perforacion;
        return(true);
    } 

    return (false);
    
}
//------------------------------------------------------------------------------
bool config_rxslotWidth(char *s_rxslotWidth)
{
    systemConf.lora_rxslot_width = atoi(s_rxslotWidth);
    return (true);
}
//------------------------------------------------------------------------------
bool config_rxslotSpread(char *s_rxslotSpread)
{
    systemConf.lora_rxslot_spread = atoi(s_rxslotSpread);
    return (true);
}
//------------------------------------------------------------------------------

void test_debug(void)
{
    
    lBchar_CreateStatic(&lora_tx_sdata, &lora_tx_buffer[0], LORA_TX_BUFFER_SIZE );
    lBchar_Flush(&lora_tx_sdata);
    sprintf( lBchar_get_buffer(&lora_tx_sdata), "mac pause" );
    xfprintf( fdTERM, "[%s]\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    xfprintf( fdLORA, "%s\r\n", lBchar_get_buffer(&lora_tx_sdata) );
    
    
}