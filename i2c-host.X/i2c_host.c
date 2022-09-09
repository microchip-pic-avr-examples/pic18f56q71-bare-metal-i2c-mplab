#include <xc.h>    

#include <stdint.h>
#include <stdbool.h>

#include "i2c_host.h"

//Initializes the I2C Module in Host Mode
//I/O is configured seperately
void I2C_initHost(void)
{
    //Enable 7-bit Host Mode
    I2C1CON0 = 0x00;
    I2C1CON0bits.MODE = 0b100;

    //Set ACKCNT to NACK when I2C1CNT = 0 during Receive
    I2C1CON1bits.ACKCNT = 1;
    
    //Default Settings - Auto-Load OFF, Address Buffers ON, 300ns Hold Time, 8 CLKs for BUSFREE
    I2C1CON2 = 0x00;
    
    //Default Settings - Standard Mode, ACNT = bits[7:0]
    I2C1CON3 = 0x00;
    
    //Disable Interrupts
    I2C1PIE = 0x00;
    
    //Clear all Interrupt Flags
    I2C1PIR = 0x00;
    
    //Clear error flags
    I2C1ERR = 0x00;
    
    //Select HFINTOSC as a clock source
    I2C1CLK = 0b00010;
    
    //100kHz Clock from a 4 MHz oscillator
    I2C1BAUD = 8;
    
    //---------------------------------------
    //TODO: Implement Bus Timeout!
    I2C1BTO = 0x00;
    //---------------------------------------
    
    //Enable I2C Module
    I2C1CON0bits.EN = 1;
}

//Initializes the I/O pins for I2C
void I2C_initPins(void)
{
    //RC3 - SCL
    //RC4 - SDA
    
    //Disable Analog Inputs
    ANSELCbits.ANSELC3 = 0;
    ANSELCbits.ANSELC4 = 0;
    
    //Configure as Open-Drain
    ODCONCbits.ODCC3 = 1;
    ODCONCbits.ODCC4 = 1;
    
    //Set as Outputs
    TRISC3 = 0;
    TRISC4 = 0;
    
    //Set PPS Outputs
    I2C1SCLPPS = 0b010011;
    I2C1SDAPPS = 0b010100;
    
    //Set PPS Inputs
    RC3PPS = 0x20;
    RC4PPS = 0x21;
    
    //Enable Internal Pullup resistors
#ifdef USE_INTERNAL_PULLUPS
    
    //Standard Slew Rate
    RC3I2Cbits.SLEW = 0b00;
    RC4I2Cbits.SLEW = 0b00;
    
    //10x Normal Pullup Strength
    RC3I2Cbits.PU = 0b10;
    RC4I2Cbits.PU = 0b10;
    
    //I2C Thresholds
    RC3I2Cbits.TH = 0b01;
    RC4I2Cbits.TH = 0b01;
#endif
}

//Attempts to send 1 byte of DATA to a device at ADDR
//Returns true if successful, or false if an error occurred
bool I2C_sendByte(uint8_t addr, uint8_t data)
{
    //Load Address
    I2C1ADB1 = (addr << 1);
    
    //Load Data Byte
    I2C1TXB = data;
    
    //Set Data Length
    I2C1CNTL = 1;
    
    //Send Start Condition
    I2C1CON0bits.S = 1;
    
    //Wait for the module to become inactive
    while (I2C1STAT0bits.MMA)
    {
        
    }
        
    //Return Status
    return (I2C1CNTL == 0);
}

//Attempts to read 1 byte of DATA from a device at ADDR
//Returns true if successful, or false if an error occurred
bool I2C_readByte(uint8_t addr, uint8_t* data)
{
    return I2C_readBytes(addr, data, 1);
}


//Addresses a device at ADDR and reads 1 byte. Returns 0x00 if NACKed
uint8_t I2C_readByteNoWarn(uint8_t addr)
{
    uint8_t data = 0x00;
    I2C_readBytes(addr, &data, 1);
    return data;
}

//Attempts to send 1 byte of data REGADDR to the device at ADDR, then restarts and reads LEN bytes to READDATA
//Returns true if successful, or false if an error occurred
bool I2C_registerWriteRead(uint8_t addr, uint8_t regAddr, uint8_t* readData, uint8_t len)
{    
    //Load Address
    I2C1ADB1 = (addr << 1);
    
    //Load Data Byte
    I2C1TXB = regAddr;
    
    //Set Data Length
    I2C1CNTL = 1;

    I2C1STAT1 = 0x00;
    I2C1ERR = 0x00;
        
    //Set Restart Enable
    I2C1CON0bits.RSEN = 1;
    
    //Start Communication
    I2C1CON0bits.S = 1;
    
    uint8_t index = 0;
    
    //While in host mode...
    while (I2C1STAT0bits.MMA)
    {
        if (I2C1CON0bits.MDR)
        {
            if (I2C1STAT1bits.RXBF)
            {
                //Read Data
                readData[index] = I2C1RXB;
                index++;
            }
            else
            {
                //Set Read address
                I2C1ADB1 |= 0b1;
                
                //Set # of Bytes
                I2C1CNTL = len;

                //Start Communication
                I2C1CON0bits.S = 1;

                //Clear Restart
                I2C1CON0bits.RSEN = 0;
                
            }
        }
        
    }
    
    if (I2C1STAT1bits.RXBF)
    {
        //Read last byte
        readData[index] = I2C1RXB;
        index++;
    }
    
    return (I2C1CNTL == 0);

}

//Attempts to send LEN bytes of DATA to the I2C with address ADDR
//Returns true if successful, or false if an error occurred
bool I2C_sendBytes(uint8_t addr, uint8_t* data, uint8_t len)
{
    //Load Address
    I2C1ADB1 = (addr << 1);
    
    //Load 1st Byte
    I2C1TXB = data[0];
    
    //Set Data Length
    I2C1CNTL = len;
                
    //Send Start Condition
    I2C1CON0bits.S = 1;
    
    uint8_t index = 1;
    
    while (I2C1STAT0bits.MMA)
    {
        if (I2C1CON0bits.MDR)
        {
            if (I2C1STAT1bits.TXBE)
            {
                //Load next byte
                I2C1TXB = data[index];
                index++;
            }
        }
    }
        
    return (I2C1CNTL == 0);
}

//Attempts to read LEN bytes of DATA from the I2C with address ADDR
//Returns true if successful, or false if an error occurred
bool I2C_readBytes(uint8_t addr, uint8_t* data, uint8_t len)
{
    //Load Address
    I2C1ADB1 = ((addr << 1) | 0b1);
    
    //Set Data Length
    I2C1CNTL = len;
        
    //Start Communication
    I2C1CON0bits.S = 1;
        
    uint8_t index = 0;
    
    //While in host mode...
    while (I2C1STAT0bits.MMA)
    {
        if (I2C1STAT1bits.RXBF)
        {
            //Read Data
            data[index] = I2C1RXB;
            index++;
        }
    }
    
    return (I2C1CNTL == 0);
}

