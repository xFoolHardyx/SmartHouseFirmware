/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"

void vTask1(void *pvParameters)
{
	volatile unsigned long ul;
	for (;;)
	{
		for (ul=0; ul<4000l; ul++){}
		vTaskDelete(NULL);
	}
}

void vTask2(void *pvParameters)
{
	volatile unsigned long ul;
	for (;;)
	{
		for (ul=0; ul<8000l; ul++){}
		vTaskDelete(NULL);
	}
}

short main(void)
{

	xTaskCreate(vTask1,(signed char *)"Task1",
			configMINIMAL_STACK_SIZE,NULL,1,NULL);
	xTaskCreate(vTask2,(signed char *)"Task2",
				configMINIMAL_STACK_SIZE,NULL,1,NULL);
	vTaskStartScheduler();
	return 0;
}
