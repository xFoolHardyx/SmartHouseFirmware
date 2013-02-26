/* Scheduler includes. */
#include <FreeRTOS.h>
#include <task.h>

#include <AT91SAM7X256.h>

/* Constants required to handle interrupts. */
#define portTIMER_MATCH_ISR_BIT		( ( unsigned char ) 0x01 )
#define portCLEAR_VIC_INTERRUPT		( ( unsigned long ) 0 )

/* Constants required to handle critical sections. */
#define portNO_CRITICAL_NESTING		( ( unsigned long ) 0 )
volatile unsigned long ulCriticalNesting = 9999UL;

/*-----------------------------------------------------------*/

/* ISR to handle manual context switches (from a call to taskYIELD()). */
void vPortYieldProcessor( void ) __attribute__((interrupt("SWI"), naked));

/* 
 * The scheduler can only be started from ARM mode, hence the inclusion of this
 * function here.
 */
void vPortISRStartFirstTask( void );
/*-----------------------------------------------------------*/

void vPortISRStartFirstTask( void )
{
	/* Simply start the scheduler.  This is included here as it can only be
	called from ARM mode. */

	extern volatile void * volatile pxCurrentTCB;
	extern volatile unsigned portLONG ulCriticalNesting;

		/* Set the LR to the task stack. */
		asm volatile (
		"LDR		R0, =pxCurrentTCB								\n\t"
		"LDR		R0, [R0]										\n\t"
		"LDR		LR, [R0]										\n\t"

		/* The critical nesting depth is the first item on the stack. */
		/* Load it into the ulCriticalNesting variable. */
		"LDR		R0, =ulCriticalNesting							\n\t"
		"LDMFD	LR!, {R1}											\n\t"
		"STR		R1, [R0]										\n\t"

		/* Get the SPSR from the stack. */
		"LDMFD	LR!, {R0}											\n\t"
		"MSR		SPSR, R0										\n\t"

		/* Restore all system mode registers for the task. */
		"LDMFD	LR, {R0-R14}^										\n\t"
		"NOP														\n\t"

		/* Restore the return address. */
		"LDR		LR, [LR, #+60]									\n\t"
		"NOP \n\t"
		/* And return - correcting the offset in the LR to obtain the */
		/* correct address. */
		"SUBS	PC, LR, #4											\n\t"
		);
		( void ) ulCriticalNesting;
		( void ) pxCurrentTCB;
	//portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

/*
 * Called by portYIELD() or taskYIELD() to manually force a context switch.
 *
 * When a context switch is performed from the task level the saved task 
 * context is made to look as if it occurred from within the tick ISR.  This
 * way the same restore context function can be used when restoring the context
 * saved from the ISR or that saved from a call to vPortYieldProcessor.
 */
void vPortYieldProcessor( void )
{
	/* Within an IRQ ISR the link register has an offset from the true return 
	address, but an SWI ISR does not.  Add the offset manually so the same 
	ISR return code can be used in both cases. */
	asm volatile ( "ADD		LR, LR, #4" );

	/* Perform the context switch.  First save the context of the current task. */
	portSAVE_CONTEXT();

	/* Find the highest priority task that is ready to run. */
	vTaskSwitchContext();

	/* Restore the context of the new task. */
	portRESTORE_CONTEXT();	
}
/*-----------------------------------------------------------*/

/* 
 * The ISR used for the scheduler tick depends on whether the cooperative or
 * the preemptive scheduler is being used.
 */

#if configUSE_PREEMPTION == 0

	/* The cooperative scheduler requires a normal IRQ service routine to 
	simply increment the system tick. */
	void vNonPreemptiveTick( void ) __attribute__ ((interrupt ("IRQ")));
	void vNonPreemptiveTick( void )
	{		
		unsigned long ulDummy;
		
		/* Increment the tick count - which may wake some tasks but as the
		preemptive scheduler is not being used any woken task is not given
		processor time no matter what its priority. */
		vTaskIncrementTick();
		
		/* Clear the PIT interrupt. */
		ulDummy = AT91C_BASE_PITC->PITC_PIVR;
		
		/* End the interrupt in the AIC. */
		AT91C_BASE_AIC->AIC_EOICR = ulDummy;
	}

#else

	/* The preemptive scheduler is defined as "naked" as the full context is
	saved on entry as part of the context switch. */
	void vPreemptiveTick( void ) __attribute__((naked));
	void vPreemptiveTick( void )
	{
		/* Save the context of the current task. */
		portSAVE_CONTEXT();

		/* Increment the tick count - this may wake a task. */
		vTaskIncrementTick();

		/* Find the highest priority task that is ready to run. */
		vTaskSwitchContext();
		
		/* End the interrupt in the AIC. */
		AT91C_BASE_AIC->AIC_EOICR = AT91C_BASE_PITC->PITC_PIVR;;
		
		portRESTORE_CONTEXT();
	}

#endif
/*-----------------------------------------------------------*/

/*
 * The interrupt management utilities can only be called from ARM mode.  When
 * THUMB_INTERWORK is defined the utilities are defined as functions here to
 * ensure a switch to ARM mode.  When THUMB_INTERWORK is not defined then
 * the utilities are defined as macros in portmacro.h - as per other ports.
 */
void vPortDisableInterruptsFromThumb( void ) __attribute__ ((naked));
void vPortEnableInterruptsFromThumb( void ) __attribute__ ((naked));

void vPortDisableInterruptsFromThumb( void )
{
	asm volatile ( 
		"STMDB	SP!, {R0}		\n\t"	/* Push R0.									*/
		"MRS	R0, CPSR		\n\t"	/* Get CPSR.								*/
		"ORR	R0, R0, #0xC0	\n\t"	/* Disable IRQ, FIQ.						*/
		"MSR	CPSR, R0		\n\t"	/* Write back modified value.				*/
		"LDMIA	SP!, {R0}		\n\t"	/* Pop R0.									*/
		"BX		R14" );					/* Return back to thumb.					*/
}
		
void vPortEnableInterruptsFromThumb( void )
{
	asm volatile ( 
		"STMDB	SP!, {R0}		\n\t"	/* Push R0.									*/	
		"MRS	R0, CPSR		\n\t"	/* Get CPSR.								*/	
		"BIC	R0, R0, #0xC0	\n\t"	/* Enable IRQ, FIQ.							*/	
		"MSR	CPSR, R0		\n\t"	/* Write back modified value.				*/	
		"LDMIA	SP!, {R0}		\n\t"	/* Pop R0.									*/
		"BX		R14" );					/* Return back to thumb.					*/
}


/* The code generated by the GCC compiler uses the stack in different ways at
different optimisation levels.  The interrupt flags can therefore not always
be saved to the stack.  Instead the critical section nesting level is stored
in a variable, which is then saved as part of the stack context. */
void vPortEnterCritical( void )
{
	/* Disable interrupts as per portDISABLE_INTERRUPTS(); 							*/
	asm volatile ( 
		"STMDB	SP!, {R0}			\n\t"	/* Push R0.								*/
		"MRS	R0, CPSR			\n\t"	/* Get CPSR.							*/
		"ORR	R0, R0, #0xC0		\n\t"	/* Disable IRQ, FIQ.					*/
		"MSR	CPSR, R0			\n\t"	/* Write back modified value.			*/
		"LDMIA	SP!, {R0}" );				/* Pop R0.								*/

	/* Now interrupts are disabled ulCriticalNesting can be accessed 
	directly.  Increment ulCriticalNesting to keep a count of how many times
	portENTER_CRITICAL() has been called. */
	ulCriticalNesting++;
}

void vPortExitCritical( void )
{
	if( ulCriticalNesting > portNO_CRITICAL_NESTING )
	{
		/* Decrement the nesting count as we are leaving a critical section. */
		ulCriticalNesting--;

		/* If the nesting level has reached zero then interrupts should be
		re-enabled. */
		if( ulCriticalNesting == portNO_CRITICAL_NESTING )
		{
			/* Enable interrupts as per portEXIT_CRITICAL().					*/
			asm volatile ( 
				"STMDB	SP!, {R0}		\n\t"	/* Push R0.						*/	
				"MRS	R0, CPSR		\n\t"	/* Get CPSR.					*/	
				"BIC	R0, R0, #0xC0	\n\t"	/* Enable IRQ, FIQ.				*/	
				"MSR	CPSR, R0		\n\t"	/* Write back modified value.	*/	
				"LDMIA	SP!, {R0}" );			/* Pop R0.						*/
		}
	}
}

