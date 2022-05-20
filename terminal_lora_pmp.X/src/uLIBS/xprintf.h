/* 
 * File:   xprintf.h
 * Author: pablo
 *
 * Created on 8 de marzo de 2022, 10:55 AM
 */

#ifndef XPRINTF_H
#define	XPRINTF_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "avr/pgmspace.h"
    
    
void putch(char c);
void XPRINTF_init(void);
int xprintf( const char *fmt, ...);
int xprintf_P( PGM_P fmt, ...);
int xfprintf( int fd, const char *fmt, ...);
void xputChar(unsigned char c);
int xputs( const char *str );


#ifdef	__cplusplus
}
#endif

#endif	/* XPRINTF_H */

