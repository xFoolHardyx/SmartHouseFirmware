#include <TWI_create_test.h>

void vTaskTWICreate(void)
{
	vTWIInit();
	xTaskCreate(vTaskTWIStart, "TWI task start", 1000, NULL, 1, NULL);
}

void vTWIInit(void)
{
	AT91F_TWI_Open();
}

void vTaskTWIStart(*pvParameters)
{

}
