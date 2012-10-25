/*
	FreeRTOS.org V4.1.0 - Copyright (C) 2003-2006 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

	***************************************************************************
	See http://www.FreeRTOS.org for documentation, latest information, license
	and contact details.  Please ensure to read the configuration and relevant
	port sections of the online documentation.
	***************************************************************************
*/

/*
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/

/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 *
 * Main.c also creates a task called "Check".  This only executes every three
 * seconds but has the highest priority so is guaranteed to get processor time.
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is
 * incremented each time the task successfully completes its function.  Should
 * any error occur within such a task the count is permanently halted.  The
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
//#include <stdlib.h>
//#include <stdio.h>

/* Scheduler includes. */
#include "include/FreeRTOS.h"
#include "include/task.h"
#include "hardware/include/Board.h"

/* Demo application includes. */
#include "include/flash.h"
#include "include/queue.h" // required by serial.h

#include "include/system.h"

/* Priorities for the demo application tasks. */
#define mainLED_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )

/* Constants required by the 'Check' task. */
#define mainCHECK_TASK_LED			( 3 ) // LED4

/*
 * Configure the processor for use with the Atmel demo board.  Setup is minimal
 * as the low level init function (called from the startup asm file) takes care
 * of most things.
 */
static void prvSetupHardware( void );

const AT91PS_PIO m_pPIOA = AT91C_BASE_PIOA;

void delay (unsigned long a) {while (--a!=0);}


void vGreenBlinkTask( void *pvParametrs ) {
	    for( ;; ) {
	    	m_pPIOA->PIO_CODR = BIT17;  //set reg to 0 (led2 on)
	    	vTaskDelay(1000);             //simple delay
	    	m_pPIOA->PIO_SODR = BIT17;  //set reg to 1 (led2 off)
	    	vTaskDelay(1000);             //simple delay
	    }
	}

void vRedBlinkTask( void *pvParametrs ) {
    for( ;; ) {
    	m_pPIOA->PIO_SODR = BIT18; //set reg to 1 (led1 off)
        vTaskDelay( 1000 );
        m_pPIOA->PIO_CODR = BIT18;  //set reg to 0 (led1 on)
        vTaskDelay( 1000 );
      }
   }

int main( void )
{
	InitFrec();
	InitPerepherial();
	////////////////////////////////////////////////

	xTaskCreate( &vGreenBlinkTask,(signed char *)"GreenBlink",configMINIMAL_STACK_SIZE,
	                 NULL,1,NULL );
	xTaskCreate( &vRedBlinkTask, (signed char *)"RedBlink", configMINIMAL_STACK_SIZE,
	                 NULL,1,NULL );

	vTaskStartScheduler();
   	///////////////////////////////////////////////

   for (;;)
   {
	   m_pPIOA->PIO_CODR = BIT17;  //set reg to 0 (led2 on)
	   m_pPIOA->PIO_SODR = BIT18;  //set reg to 1 (led1 off)
	   delay(100000);             //simple delay
	   m_pPIOA->PIO_CODR = BIT18;  //set reg to 0 (led1 on)
	   m_pPIOA->PIO_SODR = BIT17;  //set reg to 1 (led2 off)
	   delay(100000);             //simple delay
   }
   return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* When using the JTAG debugger the hardware is not always initialised to
	the correct default state.  This line just ensures that this does not
	cause all interrupts to be masked at the start. */
	AT91C_BASE_AIC->AIC_EOICR = 0;

	/* Most setup is performed by the low level init function called from the
	startup asm file. */

	/* Configure the PIO Lines corresponding to LED1 to LED4 to be outputs as
	well as the UART Tx line. */
	AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, LED_MASK );

	/* Enable the peripheral clock. */
   AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, (1 << AT91C_ID_PIOA) |  /* Enable Clock for PIO    */
                                                (1 << AT91C_ID_IRQ0) |  /* Enable Clock for IRQ0   */
                                                (1 << AT91C_ID_US0)     /* Enable Clock for USART0 */
                              );

   /* Enable reset-button */
   AT91F_RSTSetMode( AT91C_BASE_RSTC , AT91C_RSTC_URSTEN );
 }
/*-----------------------------------------------------------*/

