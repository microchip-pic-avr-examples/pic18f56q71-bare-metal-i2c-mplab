#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define INTERRUPT_BASE 0x1000
    
    //Initializes vector interrupts on the devices
    void Interrupts_init(void);
    
    //Enables interrupts on the device
    void Interrupts_enable(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* INTERRUPTS_H */

