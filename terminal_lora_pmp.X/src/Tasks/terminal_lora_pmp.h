/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

//#include <xc.h> // include processor files - each processor file is guarded.  
#include <avr/io.h>

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#ifndef F_CPU
#define F_CPU 24000000
#endif


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "croutine.h"
#include "semphr.h"
#include "timers.h"
#include "limits.h"
#include "portable.h"

#include "protected_io.h"
#include "ccp.h"

#include <avr/io.h>
#include <avr/builtins.h>
#include <avr/wdt.h> 
#include <avr/pgmspace.h>
        
#include "stdbool.h"
#include "frtos-io.h"
#include "xprintf.h"
#include "lora.h"

#define FW_REV "1.0.3 beta"
#define FW_DATE "@ 20220828"
#define HW_MODELO "Terminal Lora PMP FRTOS R001 HW:AVR128DA64"
#define FRTOS_VERSION "FW:FreeRTOS V202111.00"

#define SYSMAINCLK 24

#define tkCtl_TASK_PRIORITY	 	( tskIDLE_PRIORITY + 1 )
#define tkCmd_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkLora_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkSys_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )

#define tkCtl_STACK_SIZE		384
#define tkCmd_STACK_SIZE		384
#define tkLora_STACK_SIZE		384
#define tkSys_STACK_SIZE		384

StaticTask_t tkCtl_Buffer_Ptr;
StackType_t tkCtl_Buffer [tkCtl_STACK_SIZE];

StaticTask_t tkCmd_Buffer_Ptr;
StackType_t tkCmd_Buffer [tkCmd_STACK_SIZE];

StaticTask_t tkLora_Buffer_Ptr;
StackType_t tkLora_Buffer [tkLora_STACK_SIZE];

StaticTask_t tkSys_Buffer_Ptr;
StackType_t tkSys_Buffer [tkSys_STACK_SIZE];

SemaphoreHandle_t sem_SYSVars;
StaticSemaphore_t SYSVARS_xMutexBuffer;
#define MSTOTAKESYSVARSSEMPH ((  TickType_t ) 10 )

TaskHandle_t xHandle_tkCtl, xHandle_tkCmd, xHandle_tkLora, xHandle_tkSys;

void tkCtl(void * pvParameters);
void tkCmd(void * pvParameters);
void tkLora(void * pvParameters);
void tkSys(void * pvParameters);

void reset(void);
void system_init();
void reset(void);
bool save_config_in_NVM(void);
bool load_config_from_NVM(void);
uint8_t checksum( uint8_t *s, uint16_t size );
void kick_wdt( uint8_t bit_pos);
void config_default(void);
bool config_timerpoll(char *s_timerpoll);

bool save_config_in_NVM(void);
bool load_config_from_NVM(void);

#define MAX_LENGTH 32

typedef enum { CENTRAL=0, REMOTO } node_t;

#define LINK_WINDOW_SIZE        10
#define TIMOUT_INACTIVITY_LINK  15

struct {
    uint16_t timerPoll;                 // (M) Cada cuanto lee la entrada y tx un frame
    uint8_t checksum;
} systemConf;

struct {
    uint16_t dac;
    uint16_t link_timeout;          // (S) Tiempo en secs sin actividad
    uint16_t txmited_frames;        // (M) Cantidad de frames transmitidos
} systemVars;


uint8_t sys_watchdog;

#define CMD_WDG_bm 0x01
#define CMD_WDG_bp    0
#define CTL_WDG_bm 0x02
#define CTL_WDG_bp    1
//#define LRA_WDG_bm 0x08
//#define LRA_WDG_bp    3

#define WDG_bm 0x07

#define WDG_INIT() ( sys_watchdog = WDG_bm )


#endif	/* XC_HEADER_TEMPLATE_H */

