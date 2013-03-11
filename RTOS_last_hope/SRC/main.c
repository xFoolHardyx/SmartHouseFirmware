#include <settings.h>
#include <AT91SAM7X256.h>
#include <FreeRTOS.h>
#include <task.h>
#include <dynamic.h>
#include <semtest.h>
#include <PollQ.h>
#include <BlockQ.h>
#include <HTTP_Serv.h>

#include <lib.h>

const AT91PS_PIO m_pPIOA = AT91C_BASE_PIOA;

#define mainBLOCK_Q_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_POLL_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainHTTP_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )

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

void vTask3 (void *pvParameters)
{
	unsigned int status;
	for(;;)
	{
		portENTER_CRITICAL();
			status = 0;
			status = TWI_GetStatus();
			TWI_StartRead(0,900,1);
			TWI_Stop();
			vTaskDelay(1000);
		portEXIT_CRITICAL();
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
	vInitPerepherial();

	xTaskCreate( vTask1, "LED Task 1", 100, NULL, 3, NULL );
	xTaskCreate( vTask2, "LED Task 2", 100, NULL, 3, NULL );
//	xTaskCreate( vTask3, "LED Task 2", 100, NULL, 3, NULL );

	xTaskCreate( vHTTPServerTask, ( signed char * ) "HTTP", configMINIMAL_STACK_SIZE, NULL, mainHTTP_TASK_PRIORITY, NULL );

	vStartSemaphoreTasks( tskIDLE_PRIORITY );
	vStartDynamicPriorityTasks();
	vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );

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

