#include <settings.h>
#include <AT91SAM7X256.h>
#include <FreeRTOS.h>
#include <task.h>
#include <TCP.h>

const AT91PS_PIO m_pPIOA = AT91C_BASE_PIOA;

void delay (unsigned long a) {while (--a!=0);}

void vTask1( void *pvParameters )
{
  for( ;; )
  {
	  SetBIT(m_pPIOA,BIT18);
	  vTaskDelay(100);
	  ClrBIT(m_pPIOA,BIT18);
	  vTaskDelay(100);
  }
}

void vTask2( void *pvParameters )
{
  for( ;; )
  {
	  SetBIT(m_pPIOA,BIT16);
	  vTaskDelay(1000);
	  ClrBIT(m_pPIOA,BIT16);
	  vTaskDelay(1000);
  }
}

//void vTaskTWIStart(void *pvParameters)
//{
//	const AT91PS_PIO m_pPIOA = AT91C_BASE_PIOA;
//	for(;;)
//	{
//		ClrBIT(m_pPIOA,BIT17);
//		vTaskDelay(500);
//		SetBIT(m_pPIOA,BIT17);
//		vTaskDelay(500);
//	}
//}

void main (void)
{
	InitPerepherial();

	xTaskCreate( vTask1, "LED Task 1", 1000, NULL, 2, NULL );
	xTaskCreate( vTask2, "LED Task 2", 1000, NULL, 2, NULL );

	//xTaskCreate(vTaskTWIStart, "TWI task start", 1000, NULL, 1, NULL);

	vTaskStartScheduler();

	for (;;)
	{
		ClrBIT(m_pPIOA,BIT17);
		SetBIT(m_pPIOA,BIT18);
		delay(800000);
		ClrBIT(m_pPIOA,BIT18);
		SetBIT(m_pPIOA,BIT17);
		delay(800000);
	}
}

