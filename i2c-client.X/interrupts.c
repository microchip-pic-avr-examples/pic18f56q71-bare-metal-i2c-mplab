#include "interrupts.h"

#include <xc.h>

//Initializes vector interrupts on the devices
void Interrupts_init(void)
{
    IVTBASE = INTERRUPT_BASE;
    IVTLOCKbits.IVTLOCKED = 1;
}

//Enables interrupts on the device
void Interrupts_enable(void)
{
    INTCON0bits.GIE = 1;
}