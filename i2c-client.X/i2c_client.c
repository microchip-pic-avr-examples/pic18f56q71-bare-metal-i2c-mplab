#include <xc.h>    

#include <stdint.h>
#include <stdbool.h>

#include "i2c_client.h"
#include "interrupts.h"

static void (*rxCallback)(uint8_t) = 0;
static uint8_t (*txCallback)(void) = 0;
static void (*stopCallback)(void) = 0;

//Initializes the I2C Module in Client Mode
//I/O is configured seperately
void I2C_initClient(uint8_t address)
{
    I2C1CON0 = 0x00;
    I2C1CON0bits.MODE = 0b000;
    
    //Clear any pending flags
    I2C1CON1 = 0x00;
    
    //Set to Standard Mode
    I2C1CON3 = 0x00;
    
    //Enable STOP Interrupts
    I2C1PIE = 0x00;
    I2C1PIEbits.PC1IE = 1;
    
    //Enable Address Match Interrupts
//    I2C1PIEbits.ADRIE = 1;
    
    //Clock source is Fosc (64 MHz)
    I2C1CLK = 0b00001;
    
    //100kHz from 64 MHz
    I2C1BAUD = 159;
    
    I2C1CNTL = 0xFF;

//    I2C1TXB = 0x00;

    
    //Set Client Address
    I2C1ADR0 = address << 1;
       
    //Clear any Interrupt flags
    PIR7bits.I2C1IF = 0;
    PIR7bits.I2C1RXIF = 0;
    PIR7bits.I2C1TXIF = 0;
    
    //Enable Interrupts
    PIE7bits.I2C1IE = 1;
    PIE7bits.I2C1RXIE = 1;
    PIE7bits.I2C1TXIE = 1;
    
    //Enable I2C Module
    I2C1CON0bits.EN = 1;
}

//Initialize the bus timeout feature
void I2C_initBTO(bool reset, bool prescale, uint8_t timeout, I2C_BTO_Clock clock)
{
    I2C1BTO = (reset << 7) | (prescale << 6) | (0x3F & timeout);
    I2C1BTOC = clock;
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
    
    //Standard Slew Rate
    RC3I2Cbits.SLEW = 0b00;
    RC4I2Cbits.SLEW = 0b00;
    
    //I2C Thresholds
    RC3I2Cbits.TH = 0b01;
    RC4I2Cbits.TH = 0b01;
    
    //Enable Internal Pullup resistors
#ifdef USE_INTERNAL_PULLUPS
    
    //10x Normal Pullup Strength
    RC3I2Cbits.PU = 0b10;
    RC4I2Cbits.PU = 0b10;
#endif
}

//Write Interrupt
void __interrupt(irq(I2C1TX), base(INTERRUPT_BASE)) I2C_writeISR(void)
{    
    if (txCallback != 0)
    {
        I2C1TXB = txCallback();
    }
    else
    {
        I2C1TXB = 0x00;
    }
    
    //Clear flag
    PIR7bits.I2C1TXIF = 0;
}

//Read Interrupt
void __interrupt(irq(I2C1RX), base(INTERRUPT_BASE)) I2C_readISR(void)
{
    volatile uint8_t rx = I2C1RXB;
    
    if (rxCallback != 0)
    {
        rxCallback(rx);
    }

    
    //Clear flag
    PIR7bits.I2C1RXIF = 0;
}

//General I2C Interrupt Handler
void __interrupt(irq(I2C1), base(INTERRUPT_BASE)) I2C_stopISR(void)
{
    if (I2C1PIRbits.PCIF)
    {
        //Stop Interrupt
        I2C1CNTL = 0xFF;
        
        //Clear the buffer to remove stale data from TXB
        I2C1STAT1bits.CLRBF = 1;
        
        if (stopCallback != 0)
        {
            stopCallback();
        }
        
        //Clear STOP Flag
        I2C1PIRbits.PCIF = 0;
    }
    
    //Clear General Flag
    PIR7bits.I2C1IF = 0;
}


//This function is called on an I2C Write from the Host
void I2C_assignByteWriteHandler(void (*writeHandler)(uint8_t))
{
    rxCallback = writeHandler;
}

//This function is called when the host 
void I2C_assignByteReadHandler(uint8_t (*readHandler)(void))
{
    txCallback = readHandler;
}

//This function is called when an I2C Stop Event occurs
void I2C_assignStopHandler(void (*stopHandler)(void))
{
    stopCallback = stopHandler;
}
