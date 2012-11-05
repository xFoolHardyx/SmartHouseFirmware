#include "hardware/include/AT91SAM7S128.h"

#include "include/FreeRTOS.h"
#include "include/task.h"
#include "include/system_bit.h"


static void prvSetupHardware(void);
static void prtSeting(void);

const AT91PS_PMC 	p_pPMC 	= AT91C_BASE_PMC;  // PMC for perepherial
const AT91PS_PIO	p_pPIO	= AT91C_BASE_PIOA; // Base PIOA
const AT91PS_PIO 	m_pPIOA = AT91C_BASE_PIOA;


void TaskStartTest(void *pvParametrs)
{
	unsigned long ul;
	for(;;)
	{
		m_pPIOA->PIO_CODR = BIT17;  //set reg to 0 (led2 on)
		m_pPIOA->PIO_SODR = BIT18;
		for (ul=0;ul<4000;ul++){}
		m_pPIOA->PIO_CODR = BIT18;  //set reg to 0 (led1 on)
		m_pPIOA->PIO_SODR = BIT17;  //set reg to 1 (led2 off)
		for (ul=0;ul<4000;ul++){}
	}
}

void main (void)
{
	prvSetupHardware();
	prtSeting();
	xTaskCreate(&TaskStartTest,(signed char *) "test", configMINIMAL_STACK_SIZE , NULL, configMAX_PRIORITIES-1, NULL);
	vTaskStartScheduler();

	for (;;){

	}
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

static void prtSeting(void)
{
	/**** LED BUTTONS ****/
		//enable the clock of the PIO
		p_pPMC->PMC_PCER = 1 << AT91C_ID_PIOA; //PMC Peripheral Clock Enable Register

		//LED 1
		  //configure the PIO Lines corresponding to LED1
		  p_pPIO->PIO_PER |= BIT18;    //Enable PA17
		  p_pPIO->PIO_OER |= BIT18;    //Configure in Output
		  p_pPIO->PIO_CODR |= BIT18;   //set reg to 0

		  //LED 2
		  //configure the PIO Lines corresponding to LED2
		  p_pPIO->PIO_PER |= BIT17;    //Enable PA18
		  p_pPIO->PIO_OER |= BIT17;    //Configure in Output
		  p_pPIO->PIO_CODR |= BIT17;   //set reg to 0

}
