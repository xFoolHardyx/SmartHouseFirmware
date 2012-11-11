#include <settings.h>
#include <AT91SAM7X256.h>

void main (void)
{
	InitFrec();
	InitPerepherial();

	void delay (unsigned long a) {while (--a!=0);}

	const AT91PS_PIO m_pPIOA = AT91C_BASE_PIOA;
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

