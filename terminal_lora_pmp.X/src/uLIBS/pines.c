
#include "pines.h"

// -----------------------------------------------------------------------------
uint8_t pinputs_read(uint8_t pin_id)
{
    
uint8_t pin = 0;
    
    switch(pin_id) {
        case 0: 
            pin = ( LPIN0_PORT.IN & LPIN0_PIN_bm ) >> LPIN0;
            break;
        case 1:
            pin = ( LPIN1_PORT.IN & LPIN1_PIN_bm ) >> LPIN1;
            break;
        case 2:
            pin = ( LPIN2_PORT.IN & LPIN2_PIN_bm ) >> LPIN2;
            break;
        case 3:
            pin = ( LPIN3_PORT.IN & LPIN3_PIN_bm ) >> LPIN3;
            break;
        case 4:
            pin = ( LPIN4_PORT.IN & LPIN4_PIN_bm ) >> LPIN4;
            break;
        case 5:
            pin = ( LPIN5_PORT.IN & LPIN5_PIN_bm ) >> LPIN5;
            break;
        case 6:
            pin = ( LPIN6_PORT.IN & LPIN6_PIN_bm ) >> LPIN6;
            break;
        case 7:
            pin = ( LPIN7_PORT.IN & LPIN7_PIN_bm ) >> LPIN7;
            break;
        case 8:
            pin = ( LPIN8_PORT.IN & LPIN8_PIN_bm ) >> LPIN8;
            break;      
    }
    return(pin);
         
}
// -----------------------------------------------------------------------------
void PINPUTS_init(void)
{
    /*
     * Inicializa los pines de las entradas
     */
    
    LPIN0_PORT.DIR &= ~LPIN0_PIN_bm;
    LPIN1_PORT.DIR &= ~LPIN1_PIN_bm;
    LPIN2_PORT.DIR &= ~LPIN2_PIN_bm;
    LPIN3_PORT.DIR &= ~LPIN3_PIN_bm;
    LPIN4_PORT.DIR &= ~LPIN4_PIN_bm;
    LPIN5_PORT.DIR &= ~LPIN5_PIN_bm;
    LPIN6_PORT.DIR &= ~LPIN6_PIN_bm;
    LPIN7_PORT.DIR &= ~LPIN7_PIN_bm;
	LPIN8_PORT.DIR &= ~LPIN8_PIN_bm;
    
}
// -----------------------------------------------------------------------------
void ACGEN_init(void)
{
	// Configura los pines como output
    
	AC0_PORT.DIR |= AC0_PIN_bm;	
    
	CLEAR_AC0();

	AC1_PORT.DIR |= AC1_PIN_bm;	
	CLEAR_AC1();
  
}
// -----------------------------------------------------------------------------
bool ACGEN_set_pin( uint8_t pin_id, uint8_t pin_val )
{
       
    /*
     * El valor es el que aparece en el transformador
     */
    
    if ((pin_id == 0) && ( pin_val == 0) ) {
        CLEAR_AC0();
        return(true);
    }

    if ((pin_id == 0) && ( pin_val == 1) ) {
        SET_AC0();
        return(true);
    }

    if ((pin_id == 1) && ( pin_val == 0) ) {
        CLEAR_AC1();
        return(true);
    }

    if ((pin_id == 1) && ( pin_val == 1) ) {
        SET_AC1();
        return(true);
    }

    return(false);
    
}
// -----------------------------------------------------------------------------
