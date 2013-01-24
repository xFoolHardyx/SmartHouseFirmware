#include <settings.h>
#include <AT91SAM7X256.h>
#include <FreeRTOS.h>
#include <task.h>
#include <TWI_create_test.h>

const AT91PS_PIO m_pPIOA = AT91C_BASE_PIOA;


void delay (unsigned long a) {while (--a!=0);}

void vTask1( void *pvParameters )
{
volatile unsigned long ul;
  for( ;; )
  {

	  SetBIT(m_pPIOA,BIT18);
	  //for(ul = 0; ul < 60000; ul++ ) {}
	  vTaskDelay(100);
	  ClrBIT(m_pPIOA,BIT18);
//	  for(ul = 0; ul < 60000; ul++ ) {}
	  vTaskDelay(100);
  }
}
void vTask2( void *pvParameters )
	{
	volatile unsigned long ul;
	  for( ;; )
	  {
		 ClrBIT(m_pPIOA,BIT17);
	     //for(ul = 0; ul < 60000; ul++ ) {}
		 vTaskDelay(500);
	     SetBIT(m_pPIOA,BIT17);
	     vTaskDelay(500);
	     //for(ul = 0; ul < 60000; ul++ ) {}
	  }
	}
void vTask3( void *pvParameters )
{
volatile unsigned long ul;

  for( ;; )
  {

	  SetBIT(m_pPIOA,BIT16);
//	  for(ul = 0; ul < 60000; ul++ ) {}
	  vTaskDelay(1000);
	  ClrBIT(m_pPIOA,BIT16);
	  vTaskDelay(1000);
//	  for(ul = 0; ul < 60000; ul++ ) {}
  }
}

void main (void)
{
//	InitFrec();
	InitPerepherial();

	xTaskCreate( vTask1, "Task 1", 1000, NULL, 2, NULL );
	//xTaskCreate( vTask2, "Task 2", 1000, NULL, 2, NULL );
	xTaskCreate( vTask3, "Task 3", 1000, NULL, 2, NULL );

	vTaskTWICreate();
	xTaskCreate(vTaskTWIStart, "TWI task start", 1000, NULL, 1, NULL);

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

