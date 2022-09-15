/*
� [2022] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

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
    
    //Initializes the I2C Module in Host Mode
    //I/O is configured seperately
    void I2C_initHost(void);
    
    //Initialize the bus timeout feature
    //Reset - enables whether the I2C module should reset on a timeout
    //Prescale - enables a 32x clock divider for the timeout
    //Timeout - the number of clock cycles before a timeout. Must be less than 64
    //Clock - Select the clock source used
    void I2C_initBTO(bool reset, bool prescale, uint8_t timeout, I2C_BTO_Clock clock);
    
    //Initializes the I/O pins for I2C
    void I2C_initPins(void);

    //Attempts to send 1 byte of DATA to a device at ADDR
    //Returns true if successful, or false if an error occurred
    bool I2C_sendByte(uint8_t addr, uint8_t data);
    
    //Attempts to read 1 byte of DATA from a device at ADDR
    //Returns true if successful, or false if an error occurred
    bool I2C_readByte(uint8_t addr, uint8_t* data);
    
    //Addresses a device at ADDR and reads 1 byte. Returns 0x00 if an error occurs
    uint8_t I2C_readByteNoWarn(uint8_t addr);
    
    //Attempts to send 1 byte of data REGADDR to the device at ADDR, then restarts and reads LEN bytes to READDATA
    //Returns true if successful, or false if an error occurred
    bool I2C_registerWriteRead(uint8_t addr, uint8_t regAddr, uint8_t* readData, uint8_t len);
    
    //Attempts to send LEN bytes of DATA to a device at ADDR
    //Returns true if successful, or false if an error occurred
    bool I2C_sendBytes(uint8_t addr, uint8_t* data, uint8_t len);
    
    //Attempts to read LEN bytes of DATA from a device at ADDR
    //Returns true if successful, or false if an error occurred
    bool I2C_readBytes(uint8_t addr, uint8_t* data, uint8_t len);
    
#ifdef	__cplusplus
}
#endif

#endif	/* I2C_HOST_H */

