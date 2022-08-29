/*
 * File:   frtos10_main.c
 * Author: pablo
 *
 * Created on 25 de octubre de 2021, 11:55 AM
 * 
 * Debido a que es un micro nuevo, conviene ir a https://start.atmel.com/ y
 * crear un projecto con todos los perifericos que usemos y bajar el codigo
 * para ver como se inicializan y se manejan.
 * 
 * -----------------------------------------------------------------------------
 * Version 1.0.0 @ 2022-05-18
 * Basado en el firmware de Terminal Lora PMP
 * Usa el nuevo driver de UART
 * La terminal esta en la UART4
 * 
 * issue01: No funciona el comando reset
 *  Habia un bug nn la funcion reset() de usar un bit eqivicado como mascara.
 * 
 * issue02: No imprime caracters de un buffer de ram:
 *  https://www.microchip.com/forums/m816523.aspx
 *  https://electronics.stackexchange.com/questions/283148/printing-variable-characters-to-uart-does-not-work-constants-work-fine
 *  Implemento la funcion printf.c/printf.h. Estas me implementan las fuciones vprintf_
 *  que las uso en mi libreria xprintf.
 *  Ahora si imprimen todos los formatos en RAM.
 *  Para imprimir formatos en ROM debo imlementar vprintf_P_.
 * 
 *  En las funciones xprintf, al invocar la funcion vsnprintf no usar strlen sino sizeof.
 *  De otro modo no se imprime cuando el buffer esta vacio !!
 * 
 *  Como no puedo implementar printf_P, migro el sistema para compilarlo con AVR.
 *  Copio el proyecto y lo renombro para luego reconfigurarlo.
 *  Con este compilador SI puedo trabajar con los printf en RAM y ROM por lo que 
 *  ya no necesito el printf.c
 * 
 * issue03: En el modo cmd, no imprime la linea anterior al presionar flecha arriba
 * Se soluciono al arreglar el tema del printf (issue02)
 * 
 * issue04: No funciona un timer de rtos.(para generar la onda cuadrada del sensor)
 * Al cambiar el compilador quedo andando 'mágicamente'
 * 
 * issue05: Agregamos una tarea que monitore las entradas y el uso desde cmdLine
 * 
 * 
 * 
 * -----------------------------------------------------------------------------
 * Version 5: Implemento una funcion que lee la entrada y genera argumentos
 * En el tkComd (04) implemento que write escriba variables y con read poder 
 * leerlas.
 * No puedo tener el tickless habilitado porque no lee.!!
 * Problemas con strings.
 * - Si tengo un string inicializado, SI lo puedo imprimir.
 * - Si lo inicializo, NO se imprime.
 * No se si el problema esta en la inicializacion o en la impresión !!
 * - Confirmo que la copia funciona OK. Si luego imprimo como puntero funciona.
 * - Hago una funcion xputs que imprime directamente un string y funciona bien
 * lo que confirma que el problema esta en el xprintf("%s")
 * El problema está en la funcion vsnprinf que viene en el XC8.
 * Por eso usamos printf.c y la funcion vsnprintf_
 * 
 * 
 * ----------------------------------------------------------------------------- 
 * Version 4: Implemento el driver de uart para escritura.
 * Requieren la libreria de ringBuffers
 * Con esto la trasmision / recepcion es por interrupcion.
 * Implemento tambien la capa FRTOS-IO
 * A) Probamos el write sin tickless. Escritura base OK.
 * B) Paso a tickless: Funciona correctamente agregando el _delay_ms al final del
 * xprintf.
 * C) Probamos generar valores de variables globales en una tarea y leerlas y
 * mostrarlas en otra.
 * Generamos un uint8_t, int16_t, uint16_t, float, char, string
 * uint8_t: OK
 * int16_t: OK
 * uint16_t: OK
 * char: OK
 * float: OK
 * string: OK
 * 
 * Para que imprima floats hay que poner en el linker options la opcion
 * -Wl,-u,vfprintf -lprintf_flt -lm
 * https://github.com/microchip-pic-avr-examples/avr128da48-cnano-printf-float-mplab-mcc
 * 
 * C) Probamos con strings y formatos en PSTR
 * - El strncpy_P no funciona bien.
 * 
 * -----------------------------------------------------------------------------
 *  * Version 3: Implementamos una salida serial directa con printf.
 * Agregamos el directorio Drivers con los archivos usart.c y usart.h donde
 * implemento las funciones de lectura/escritura de los puertos seriales 0 y 4.
 * El manejo es por poleo.
 * La placa donde estamos probando usa el puerto 4.
 * Creo una nueva tarea tk3 que se va a encargar de imprimir un string y donde vamos
 * a probar las funciones de printf.
 * Utilizo la funcion vsnprintf del XC8
 * Formatea bien pero se come el ultimo caracter a veces.
 * El problema es que al configurar para TICKLESS, luego de poner el ultimo
 * byte se duerme y por eso a veces no lo transmite.
 * Para evitarlo, luego de transmitir el buffer debo agregar un busy-loop
 * con _delay_ms(50) de modo que no entre en sleep y pueda vaciarse los buffers.
 * 
 * 
 * -----------------------------------------------------------------------------
 * Version 2: Modo Tickless
 * Al codigo anterior le agregamos en FreeRTOSConfig.h la linea
 * #define configUSE_TICKLESS_IDLE     1
 * 
 * -----------------------------------------------------------------------------
 * Version 1: Modo normal ( no pwrsave )
 * Tiene 2 tareas:
 * - Una resetea el watchdog c/5s.
 * - La otra prende y apaga el led.
 * 
 * -----------------------------------------------------------------------------
 * Version 0 @ 2022-03-07.
 * Este proyecto es para probar el funcionamiento del FRTOS en la plataforma 
 * AVR128DA64 que es la que usamos en nuestros desarrollos
 * En particular lo ajustamos a la placa del sensor de cloro.
 * 
 * 
 * El resultado es que la placa "sensor de cloro" reduce el consumo de 11 a 7mA.
 * ( la placa no esta optimizada para reducir el consumo ).
 *  
 */

#include <avr/io.h>
#include "terminal_lora_pmp.h"

FUSES = {
	.WDTCFG = 0x0B, // WDTCFG {PERIOD=8KCLK, WINDOW=OFF}
	.BODCFG = 0x00, // BODCFG {SLEEP=DISABLE, ACTIVE=DISABLE, SAMPFREQ=128Hz, LVL=BODLEVEL0}
	.OSCCFG = 0xF8, // OSCCFG {CLKSEL=OSCHF}
	.SYSCFG0 = 0xD2, // SYSCFG0 {EESAVE=CLEAR, RSTPINCFG=GPIO, CRCSEL=CRC16, CRCSRC=NOCRC}
	.SYSCFG1 = 0xF8, // SYSCFG1 {SUT=0MS}
	.CODESIZE = 0x00, // CODESIZE {CODESIZE=User range:  0x0 - 0xFF}
	.BOOTSIZE = 0x00, // BOOTSIZE {BOOTSIZE=User range:  0x0 - 0xFF}
};

LOCKBITS = 0x5CC5C55C; // {KEY=NOLOCK}


//------------------------------------------------------------------------------
int main(void) {

    system_init();
    frtos_open(fdTERM, 9600 );
    frtos_open(fdLORA, 57600 );
    frtos_open(fdI2C, 100 );
    frtos_open(fdNVM, 0 );
    
    sem_SYSVars = xSemaphoreCreateMutexStatic( &SYSVARS_xMutexBuffer );
    sem_LORA = xSemaphoreCreateMutexStatic( &LORA_xMutexBuffer );
    
    xHandle_tkCtl = xTaskCreateStatic( tkCtl, "CTL", tkCtl_STACK_SIZE, (void *)1, tkCtl_TASK_PRIORITY, tkCtl_Buffer, &tkCtl_Buffer_Ptr );
    xHandle_tkCmd = xTaskCreateStatic( tkCmd, "CMD", tkCmd_STACK_SIZE, (void *)1, tkCmd_TASK_PRIORITY, tkCmd_Buffer, &tkCmd_Buffer_Ptr );
    xHandle_tkLora = xTaskCreateStatic( tkLora, "LORA", tkLora_STACK_SIZE, (void *)1, tkLora_TASK_PRIORITY, tkLora_Buffer, &tkLora_Buffer_Ptr );
    xHandle_tkSys = xTaskCreateStatic( tkSys, "SYS", tkSys_STACK_SIZE, (void *)1, tkSys_TASK_PRIORITY, tkSys_Buffer, &tkSys_Buffer_Ptr );

    
    /* Arranco el RTOS. */
	vTaskStartScheduler();
  
	// En caso de panico, aqui terminamos.
	exit (1);
    
}
//------------------------------------------------------------------------------
/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
//------------------------------------------------------------------------------------
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
//-----------------------------------------------------------------------------


