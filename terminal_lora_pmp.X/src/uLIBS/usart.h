/* 
 * File:   usart.h
 * Author: pablo
 *
 * Created on 27 de octubre de 2021, 03:58 PM
 */

#ifndef USART_H
#define	USART_H

#ifdef	__cplusplus
extern "C" {
#endif


#ifdef	__cplusplus
}
#endif

#include "stdbool.h"

void USART0_init(void);
void USART0_sendChar(char c);
void USART0_sendString(char *str);
char USART0_readChar(bool echo);
bool USART0_getChar( char *c );

void USART4_init(void);
void USART4_sendChar(char c);
void USART4_sendString(char *str);
char USART4_readChar(bool echo);
bool USART4_getChar( char *c );

#endif	/* USART_H */

