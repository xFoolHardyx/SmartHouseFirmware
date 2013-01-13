#include <settings.h>
#include <AT91SAM7X256.h>
#include <FreeRTOS.h>
#include <task.h>

const AT91PS_PIO m_pPIOA = AT91C_BASE_PIOA;


void delay (unsigned long a) {while (--a!=0);}

void vTask1( void *pvParameters )
{
volatile unsigned long ul;
  for( ;; )
  {
	  ClrBIT(m_pPIOA,BIT18);
	  for(ul = 0; ul < 60000; ul++ ) {}
	  SetBIT(m_pPIOA,BIT18);
	  for(ul = 0; ul < 60000; ul++ ) {}
  }
}
void vTask2( void *pvParameters )
	{
	volatile unsigned long ul;
	  for( ;; )
	  {
		 ClrBIT(m_pPIOA,BIT17);
	     for(ul = 0; ul < 60000; ul++ ) {}
	     SetBIT(m_pPIOA,BIT17);
	     for(ul = 0; ul < 60000; ul++ ) {}
	  }
	}

void main (void)
{
//	InitFrec();
	InitPerepherial();

	xTaskCreate( vTask1, "Task 1", 1000, NULL, 1, NULL );
	xTaskCreate( vTask2, "Task 2", 1000, NULL, 1, NULL );

	vTaskStartScheduler();

	for (;;)
	{
		ClrBIT(m_pPIOA,BIT17);
		SetBIT(m_pPIOA,BIT18);
		delay(800000);             //simple delay
		ClrBIT(m_pPIOA,BIT18);
		SetBIT(m_pPIOA,BIT17);
		delay(800000);             //simple delay
	}
}

