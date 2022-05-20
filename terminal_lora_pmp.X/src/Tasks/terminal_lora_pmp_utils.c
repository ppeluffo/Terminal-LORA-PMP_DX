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
  
    systemConf.tipo_nodo = CENTRAL;
    systemConf.timerPoll = 60;
    systemConf.tx_window_size = LINK_WINDOW_SIZE;
    systemConf.an_channel_for_convert = 0;
    systemConf.max_inactivity_link = TIMOUT_INACTIVITY_LINK;
    
    systemVars.dac = 0;
    
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
bool config_modo(char *s_modo)
{
    if ( strcmp( strupr( s_modo),"CENTRAL") == 0  ) {
        systemConf.tipo_nodo = CENTRAL;
        return(true);
    }

    if ( strcmp( strupr( s_modo),"REMOTO") == 0  ) {
        systemConf.tipo_nodo = REMOTO;
        return(true);
    }
    
    return(false);
    
}
//------------------------------------------------------------------------------
bool config_timerpoll(char *s_timerpoll)
{
    systemConf.timerPoll = atoi(s_timerpoll);
    return (true);
}
//------------------------------------------------------------------------------
bool config_txwindow(char *s_txwindow)
{
    systemConf.tx_window_size = atoi(s_txwindow);
    return (true);
}
//------------------------------------------------------------------------------
bool config_ANoutputChannel(char *s_anOutputChannel)
{
    systemConf.an_channel_for_convert = atoi(s_anOutputChannel);
    return (true);
}
//------------------------------------------------------------------------------
bool config_linktimeout(char *s_linkTimeout)
{
    systemConf.max_inactivity_link = atoi(s_linkTimeout);
    return(true);
}
//------------------------------------------------------------------------------
