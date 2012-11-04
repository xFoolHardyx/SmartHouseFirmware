#include "hardware/include/AT91SAM7S128.h"

#include "include/FreeRTOS.h"

static void prvSetupHardware(void);

void main (void)
{
	prvSetupHardware();


	for (;;){}
}


static void prvSetupHardware( void )
{
	portDISABLE_INTERRUPTS();

	/* When using the JTAG debugger the hardware is not always initialised to
	the correct default state.  This line just ensures that this does not
	cause all interrupts to be masked at the start. */
	AT91C_BASE_AIC->AIC_EOICR = 0;

	/* Most setup is performed by the low level init function called from the
	startup asm file. */

	/* Enable the peripheral clock. */
    AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOA;
}
