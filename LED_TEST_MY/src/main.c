#include "system.h"
#include "AT91SAM7S128.h"

const AT91PS_PIO m_pPIOA = AT91C_BASE_PIOA;

void delay (unsigned long a) {while (--a!=0);}

int main()
{
	//INIT
	InitFrec();
	InitPerepherial();

	//loop forever
	while (1)
	{
//		 m_pPIOA->PIO_CODR = BIT17;  //set reg to 0 (led2 on)
//		 m_pPIOA->PIO_SODR = BIT18;  //set reg to 1 (led1 off)
//		 delay(800000);             //simple delay
//		 m_pPIOA->PIO_CODR = BIT18;  //set reg to 0 (led1 on)
//		 m_pPIOA->PIO_SODR = BIT17;  //set reg to 1 (led2 off)
//		 delay(800000);             //simple delay
	}
}
