/*
© [2022] Microchip Technology Inc. and its subsidiaries.

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

#ifndef I2C_BLOCKDATA_H
#define	I2C_BLOCKDATA_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>
#include <stdint.h>
    
/*
 * This defines the 1st data byte of an i2c write as the address.
 * If commented out, each byte is loaded into the array. 
 */
#define FIRST_BYTE_ADDR
    
    /**
     * <b><FONT COLOR=BLUE>void</FONT> _I2C_BlockData_StoreByte(<FONT COLOR=BLUE>uint8_t</FONT> data)</B>
     * @param uint8_t data - Byte of data received by the I2C module
     * 
     * This function stores a byte of data into the "write" buffer associated.
     * If the index internally exceeds the write buffer size, then the data is discarded.
     */
    void I2C_BlockData_StoreByte(uint8_t data);
    
    /**
     * <b><FONT COLOR=BLUE>uint8_t</FONT> _I2C_BlockData_RequestByte(<FONT COLOR=BLUE>void</FONT>)</B>
     * 
     * This function returns a byte of data from the "read" buffer at the internal index.
     * If the index internally exceeds the read buffer size, then a default value (0x00) is returned.
     */
    uint8_t I2C_BlockData_RequestByte(void);
    
    /**
     * <b><FONT COLOR=BLUE>void</FONT> _I2C_BlockData_onStop(<FONT COLOR=BLUE>void</FONT>)</B>
     * 
     * This function is called to indicate the MSSP has stopped. This resets flags
     * for the block read/write.
     */
    void I2C_BlockData_onStop(void);
        
    /**
     * <b><FONT COLOR=BLUE>void</FONT> I2C_BlockData_setupReadBuffer(<FONT COLOR=BLUE>uint8_t*</FONT> buffer, <FONT COLOR=BLUE>uint8_t</FONT> size)</B>
     * @param buffer (uint8_t*) - Buffer to read data from
     * @param size (uint8_t) - Length of the string to send.
     * 
     * Assigns the buffer of memory to read data from.
     */
    void I2C_BlockData_setupReadBuffer(volatile uint8_t* buffer, uint8_t size);
    
    /**
     * <b><FONT COLOR=BLUE>void</FONT> I2C_BlockData_setupWriteBuffer(<FONT COLOR=BLUE>uint8_t*</FONT> buffer, <FONT COLOR=BLUE>uint8_t</FONT> size)</B>
     * @param buffer (uint8_t*) - Buffer to write data to
     * @param size (uint8_t) - Length of the string to send.
     * 
     * Assigns the buffer of memory to write data to.
     */
    void I2C_BlockData_setupWriteBuffer(volatile uint8_t* buffer, uint8_t size);
    
#ifdef	__cplusplus
}
#endif

#endif	/* I2C_BLOCKDATA_H */

