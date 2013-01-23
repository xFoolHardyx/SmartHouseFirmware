#ifndef TWI_CREATE_TEST_H_
#define TWI_CREATE_TEST_H_

#include <TWI.h>

#define NULL ((void *)0)

void vTWIInit(void);
void vTaskTWICreate(void);
void vTaskTWIStart (void *pvParameters);

#endif /* TWI_CREATE_TEST_H_ */
