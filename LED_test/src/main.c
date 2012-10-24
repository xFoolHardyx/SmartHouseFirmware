//Simple program that blinking leds

#include "AT91SAM7S64.h"
#include "system.h"

//MAIN POINTERS
AT91PS_PIO    m_pPio   = AT91C_BASE_PIOA;

//it's a simple delay
void Delay (unsigned long a) { while (--a!=0); }

int main()
{
  //Init frequency
 // InitFrec();
  //Init leds
  InitPeriphery();

  // loop forever
  while(1)
  {
    m_pPio->PIO_CODR = BIT18;  //set reg to 0 (led2 on)
    m_pPio->PIO_SODR = BIT17;  //set reg to 1 (led1 off)
    Delay(800000);             //simple delay
    m_pPio->PIO_CODR = BIT17;  //set reg to 0 (led1 on)
    m_pPio->PIO_SODR = BIT18;  //set reg to 1 (led2 off)
    Delay(800000);             //simple delay
  }
}

