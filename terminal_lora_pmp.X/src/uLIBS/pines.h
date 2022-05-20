/* 
 * File:   pines.h
 * Author: pablo
 *
 * Created on 11 de febrero de 2022, 06:02 PM
 */

#ifndef PINES_H
#define	PINES_H

#ifdef	__cplusplus
extern "C" {
#endif

//#include <xc.h>
#include <avr/io.h>
#include "stdbool.h"
    
//--------------------------------------------------------------------------
// Pines generadores de senal AC para el transformador
    
#define ACPIN0  0
#define ACPIN1  1
    
#define AC0_PORT	PORTE
#define AC0_PIN_bm	PIN6_bm
#define AC0_PIN_bp	PIN6_bp
    
#define AC1_PORT	PORTE
#define AC1_PIN_bm	PIN7_bm
#define AC1_PIN_bp	PIN7_bp

    
#define SET_AC0()       ( AC0_PORT.OUT |= AC0_PIN_bm )
#define CLEAR_AC0()     ( AC0_PORT.OUT &= ~AC0_PIN_bm )
#define TOGGLE_AC0()    ( AC0_PORT.OUT ^= 1UL << AC0_PIN_bp);
    
#define SET_AC1()       ( AC1_PORT.OUT |= AC1_PIN_bm )
#define CLEAR_AC1()     ( AC1_PORT.OUT &= ~AC1_PIN_bm )
#define TOGGLE_AC1()    ( AC1_PORT.OUT ^= 1UL << AC1_PIN_bp);
    
#define LPIN0           6
#define LPIN0_PORT      PORTA
#define LPIN0_PIN_bm	PIN6_bm
#define LPIN0_PIN_bp	PIN6_bp

#define LPIN1           4
#define LPIN1_PORT      PORTA
#define LPIN1_PIN_bm	PIN4_bm
#define LPIN1_PIN_bp	PIN4_bp    
    
#define LPIN2           2
#define LPIN2_PORT      PORTB
#define LPIN2_PIN_bm	PIN2_bm
#define LPIN2_PIN_bp	PIN2_bp
    
#define LPIN3           0
#define LPIN3_PORT      PORTB
#define LPIN3_PIN_bm	PIN0_bm
#define LPIN3_PIN_bp	PIN0_bp
    
#define LPIN4           7
#define LPIN4_PORT      PORTA
#define LPIN4_PIN_bm	PIN7_bm
#define LPIN4_PIN_bp	PIN7_bp
    
#define LPIN5           5
#define LPIN5_PORT      PORTA
#define LPIN5_PIN_bm	PIN5_bm
#define LPIN5_PIN_bp	PIN5_bp
    
#define LPIN6           3
#define LPIN6_PORT      PORTB
#define LPIN6_PIN_bm	PIN3_bm
#define LPIN6_PIN_bp	PIN3_bp
    
#define LPIN7           1
#define LPIN7_PORT      PORTB
#define LPIN7_PIN_bm	PIN1_bm
#define LPIN7_PIN_bp	PIN1_bp
    
#define LPIN8           3
#define LPIN8_PORT      PORTA
#define LPIN8_PIN_bm	PIN3_bm
#define LPIN8_PIN_bp	PIN3_bp
    

void ACGEN_init(void);
bool ACGEN_set_pin( uint8_t pin_id, uint8_t pin_val);
void PINPUTS_init(void);

uint8_t pinputs_read(uint8_t pin_id);

#ifdef	__cplusplus
}
#endif

#endif	/* PINES_H */

