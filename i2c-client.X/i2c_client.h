#ifndef I2C_HOST_H
#define	I2C_HOST_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
//If defined, internal pull-up resistors will be used
#define USE_INTERNAL_PULLUPS
    
    //Options for Bus Time Out (BTO) Clock Sources
    typedef enum {
        I2C_BTO_TMR2 = 0b0001, I2C_BTO_TMR4, 
        I2C_TU16A, I2C_TU16B_OUT, I2C_BTO_LFINTOSC, 
        I2C_BTO_MFINTOSC, I2C_BTO_SOSC
    } I2C_BTO_Clock;
    
    //Initializes the I2C Module in Client Mode
    //I/O is configured seperately
    void I2C_initClient(uint8_t address);
    
    //Initialize the bus timeout feature
    //Reset - enables whether the I2C module should reset on a timeout
    //Prescale - enables a 32x clock divider for the timeout
    //Timeout - the number of clock cycles before a timeout. Must be less than 64
    //Clock - Select the clock source used
    void I2C_initBTO(bool reset, bool prescale, uint8_t timeout, I2C_BTO_Clock clock);
    
    //Initializes the I/O pins for I2C
    void I2C_initPins(void);
    
    //This function is called on an I2C Write from the Host
    void I2C_assignByteWriteHandler(void (*writeHandler)(uint8_t));
    
    //This function is called when the host 
    void I2C_assignByteReadHandler(uint8_t (*readHandler)(void));

    //This function is called when an I2C Stop Event occurs
    void I2C_assignStopHandler(void (*stopHandler)(void));

    
#ifdef	__cplusplus
}
#endif

#endif	/* I2C_HOST_H */
