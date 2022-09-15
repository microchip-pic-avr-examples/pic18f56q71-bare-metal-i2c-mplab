<!-- Please do not change this logo with link -->

[![MCHP](images/microchip.png)](https://www.microchip.com)

# Bare Metal I<sup>2</sup>C Drivers for the PIC18F56Q71 Family of MCUs

In this example, bare metal I<sup>2</sup>C drivers for the stand-alone I<sup>2</sup>C peripheral on the PIC18F56Q71 family of MCUs will be created.

## Related Examples

- [Bare Metal I<sup>2</sup>C for the PIC16F15244 (MSSP)](https://github.com/microchip-pic-avr-examples/pic16f15244-bare-metal-i2c-mplab)
- [Bare Metal TWI for AVR128DB48](https://github.com/microchip-pic-avr-examples/avr128db48-bare-metal-twi-mplab)
- [Bare Metal TWI for ATtiny1627](https://github.com/microchip-pic-avr-examples/attiny1627-bare-metal-twi-mplab)

## Software Used

- [MPLAB® X IDE v6.0.0 or newer](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic18q71&utm_content=pic18f56q71-bare-metal-i2c-mplab)
- [MPLAB XC8 v2.40.0 or newer](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic18q71&utm_content=pic18f56q71-bare-metal-i2c-mplab)

## Hardware Used

To run this project, an MCU from the PIC18F56Q71 family is required, but other devices with a (standalone) I<sup>2</sup>C should also function with adjustments.

*Important: This driver is written to use Vector Interrupts. Modifications to the ISR definitions are required to use non-vector interrupts.*

This project was tested and built on a PIC18F56Q71 Curiosity Nano.

- PIC18F56Q71 Curiosity Nano
- [Curiosity Nano Base for Click boards™ (AC164162)](https://www.microchip.com/en-us/development-tool/AC164162?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic18q71&utm_content=pic18f56q71-bare-metal-i2c-mplab)

Alternatively, this project should also be compatiable with a [Curiosity High Pin Count Board (DM164136)](https://www.microchip.com/en-us/development-tool/DM164136?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic18q71&utm_content=pic18f56q71-bare-metal-i2c-mplab) and a discrete DIP part.

**Important! External Pull-up resistors were used. Internal Pull-up resistors can be enabled by uncommenting `#define USE_INTERNAL_PULLUPS` in either `i2c_host.h` or `i2c_client.h`.**

### Host Mode Testing

To use the I<sup>2</sup>C host driver, an I<sup>2</sup>C client device is required. This can be created using another MCU in client mode, or by using a MikroElektronika (mikroE) Click board. A number of compatible Click boards are available from [mikroE](https://www.mikroe.com/click).

For testing, a PIC16F15244 running the [Advanced I/O Expander](https://github.com/microchip-pic-avr-examples/pic16f15244-family-advanced-i2c-io-expander) example was used to test host mode operation. 

*Note: During development, we found the internal pullups were not strong enough, which caused the I/O expander to not start-up reliably. The reset routine implemented will keep retrying until the board communicates.* 

### Client Mode Testing

To use the I<sup>2</sup>C client driver, a device capable of generating I<sup>2</sup>C host communication is required. This can be another MCU configured as a client, or a stand-alone device, such as an [MCP2221A USB-I<sup>2</sup>C Breakout Module (ADM00559)](https://www.microchip.com/en-us/development-tool/ADM00559?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic18q71&utm_content=pic18f56q71-bare-metal-i2c-mplab), which was used for testing.

## Pin Setup

This example uses pins RC3 and RC4 for I<sup>2</sup>C communication. These are the default pins used on the Curiosity Nano Adapter board for I<sup>2</sup>C.

| Pin | Function |
| --- | --------
| RC3 | SCL
| RC4 | SDA
| RC7 | LED0 (debug I/O)
| RF5 | I/O Expander Reset (host mode test)

## Using the I<sup>2</sup>C Host Driver  

The I<sup>2</sup>C Host Driver is a *polled* driver that can initiate communication with I<sup>2</sup>C clients. This driver is composed of 2 files: *i2c_host.h* and *i2c_host.c*  

### Initializing the Driver

The driver is initialized by calling the functions:

```
//Init I/O
I2C_initPins();

//Init I2C Module in Host Mode
I2C_initHost();
```  

Additionally, hardware Bus Timeout (BTO) features can be configured by using the function:  

`void I2C_initBTO(bool reset, bool prescale, uint8_t timeout, I2C_BTO_Clock clock)`

| Parameter | Description
| --------- | -----------
| reset | If enabled, the I<sup>2</sup>C module will reset on a BTO event. 
| prescale | Enables a 32x prescaler for the timeout. 
| timeout | Number of clock cycles to trigger a timeout. **Must be less than or equal to 63.**
| clock | Clock source enumeration for the BTO.

The BTO features protect against a stalled I<sup>2</sup>C bus. With this feature enabled, if the bus stalls, the module will reset, clearing internal hardware flags, which should free the microcontroller from the stall. In cases where the microcontroller is causing the stall, e.g.: an infinite loop, the BTO will free the bus by clearing the clock stretching flag and client mode active bits. Please consult the datasheet for more information. 

*Note: Certain clock sources with the 32x prescaler will have an ~1 ms period. Please consult the datasheet for more information.*

### Writing to Clients

There are 2 functions that write data to the client device:

```
bool I2C_sendByte(uint8_t addr, uint8_t data);
bool I2C_sendBytes(uint8_t addr, uint8_t* data, uint8_t len);
```

The `I2C_sendByte` function sends the 7-bit address in `deviceADDR` to the client. If the client ACKs this address, then `data` is sent.

Similiarly, the `I2C_sendBytes` function sends the 7-bit address in `addr` to the client. If the client ACKs this address, then `len` bytes are sent from `data`.

If the client does not ACK (NACK), then the function returns false (and no data is sent).

### Reading Data from Clients

There are 3 functions that are designed to read data from the client.

```
bool I2C_registerWriteRead(uint8_t addr, uint8_t regAddr, uint8_t* readData, uint8_t len);
bool I2C_readByte(uint8_t addr, uint8_t* data);
uint8_t I2C_readByteNoWarn(uint8_t addr);
bool I2C_readBytes(uint8_t addr, uint8_t* data, uint8_t len);
```

#### Register Select and Read

`I2C_registerWriteRead` is a special function in this driver. Unlike the other read functions, this one starts by initiating an I<sup>2</sup>C write to the device at `addr`. Then, `registerAddr` is written to the device to select the address to read from. After writing that byte, the I<sup>2</sup>C bus is restarted, and the client is readdressed, but in read mode. `len` bytes of data are read from the device and stored in `readData`.

If the client NACKs at either of the addressing steps in this function, the operation will be aborted (and the function will return false).

#### Read Byte and Block

```
bool I2C_readByte(uint8_t addr, uint8_t* data);
uint8_t I2C_readByteNoWarn(uint8_t addr);
bool I2C_readBytes(uint8_t addr, uint8_t* data, uint8_t len);
```

These functions all operate on a nearly identical basis. A client device is addressed with `addr` in read mode. Then, either a single byte or `len` bytes of data are read from the device.

No read occurs if the client NACKs communication. Boolean functions return false in this case. The function `I2C_readByteNoWarn` is a wrapper for the function `I2C_readByte`. If the client NACKs, then 0x00 is returned, rather than true or false values.

### API Functions

| Function Definition | Description
| ------------------- | --------
| void I2C_initHost(void) | Initializes the I<sup>2</sup>C module in Host Mode. I/O must also be configured with `I/O_initPins`.
| void I2C_initBTO(bool reset, bool prescale, uint8_t timeout, I2C_BTO_Clock clock) | Initializes the bus timeout features of the I<sup>2</sup>C module. 
| void I2C_initPins(void) | Initializes the I/O pins used by the I<sup>2</sup>C module.
| bool I2C_sendByte(uint8_t addr, uint8_t data) | Attempts to send 1 byte of DATA to a device at ADDR. Returns true if successful, or false if an error occurred.
| bool I2C_readByte(uint8_t addr, uint8_t* data) | Attempts to read 1 byte of DATA from a device at ADDR. Returns true if successful, or false if an error occurred.
| uint8_t I2C_readByteNoWarn(uint8_t addr) | Addresses a device at ADDR and reads 1 byte. Returns 0x00 if an error occurs.
| bool I2C_registerWriteRead(uint8_t addr, uint8_t regAddr, uint8_t* readData, uint8_t len) | Attempts to send 1 byte of data REGADDR to the device at ADDR, then restarts and reads LEN bytes to READDATA. Returns true if successful, or false if an error occurred.
| bool I2C_sendBytes(uint8_t addr, uint8_t* data, uint8_t len) | Attempts to send LEN bytes of DATA to a device at ADDR. Returns true if successful, or false if an error occurred.
| bool I2C_readBytes(uint8_t addr, uint8_t* data, uint8_t len) | Attempts to read LEN bytes of DATA from a device at ADDR. Returns true if successful, or false if an error occurred.  

## Using the I<sup>2</sup>C Client Driver

I<sup>2</sup>C clients are devices that respond to a read/write request from an I<sup>2</sup>C host. Since a host does not communicate continuously, *interrupt* driven operation is crucial for most client devices.

This driver can be used either in byte mode or block mode (via an included middleware library).

Byte Driver &lrarr; Application Code

Byte Driver &lrarr; Block Memory Middleware &lrarr; Application Code  

### Byte Mode Driver

Byte mode is designed to call a user-defined function for all byte read/write events and for the I<sup>2</sup>C stop event. If one of these user defined functions is not set (or set to NULL), the driver will perform a default action, as shown below, to maintain functionality.

| Event | Default Action
| ----- | --------------
| Read  | Writes 0x00 to the Host
| Write | Discards Received Byte
| Stop  | Does Nothing

To write a function that can be called for these events, they must have the following definition:

| Event | Required Function Definition |
| ----- | ----------------  
| Read  | uint8_t myReadFunction(void)
| Write | void myWriteFunction(uint8_t data)
| Stop  | void myStopFunction(void)

Like all interrupt handlers, the functions associated should be as small as possible and ***non-blocking***.

*Note: For Read Events, n + 1 events will occur, due to the host ACKing the communication before the STOP event. The block mode driver corrects it's index for this event.*

#### API Functions (i2c_client.h)

| Function Definition | Description
| ------------------- | --------
| void I2C_initClient(uint8_t address) | Initializes the I<sup>2</sup>C Module in Client Mode. I/O must also be configured with `I/O_initPins`.
| void I2C_initBTO(bool reset, bool prescale, uint8_t timeout, I2C_BTO_Clock clock) | Initializes the bus timeout features of the I<sup>2</sup>C module. **See host mode initialization for more information.**
| void I2C_initPins(void) | Initializes the I/O pins for the I<sup>2</sup>C module.
| void I2C_assignByteWriteHandler(void (*writeHandler)(uint8_t)) | This function is called on an I<sup>2</sup>C Write from the Host.
| void I2C_assignByteReadHandler(uint8_t (*readHandler)(void)) | This function is called when the host.
| void I2C_assignStopHandler(void (*stopHandler)(void)) | This function is called when an I<sup>2</sup>C Stop Event occurs.

### Block Mode Middleware

Block mode simplifies development by implementing multi-byte memory transfers, with support for both incremental transfers (byte 0, 1, 2, etc...) and addressed transfers (set index to 4, RESTART/STOP, read 4, read 5, read 6, etc...).

To disable addressed transfers, comment out `#define FIRST_BYTE_ADDR` in *i2c_blockData.h*.

#### Setting up Block Mode

To use the block mode middleware, it must be attached to the byte mode driver, shown below:

~~~
//Init I/O
I2C_initPins();

//Init I2C Client
I2C_initClient(0x64);

//...

//Block Mode Driver Configuration
I2C_assignByteWriteHandler(&I2C_BlockData_StoreByte);
I2C_assignByteReadHandler(&I2C_BlockData_RequestByte);
I2C_assignStopHandler(&I2C_BlockData_onStop);
~~~

The functions `I2C_BlockData_StoreByte`, `I2C_BlockData_RequestByte`, and `I2C_BlockData_onStop` are functions provided in the library. These functions are responsible for storing and retrieving bytes of data from the user defined memory blocks. The stop function (`I2C_BlockData_onStop`) is used to adjust (or reset) the memory access index.

*Note: In the event that the memory blocks are not initialized, or a read/write overflow occurs, the driver will discard any further received bytes (write) or will return 0x00 to the driver (read).*

#### Attaching Memory Blocks

This library supports separated read and write memory blocks for I<sup>2</sup>C with varying sizes between each block. The functions in the library verify the I<sup>2</sup>C does not exceed the size of each memory block.

Two functions are provided to set the blocks, as shown below.

~~~
//Setup a Read Buffer
setupReadBuffer(&buffer[0], 8);

//Setup a Write Buffer. (For testing, set to smaller than read.)
setupWriteBuffer(&buffer[0], 4);
~~~

In the above example, both the read and write buffers point to the same memory pool, however separate arrays can also be used. If the write function should only modify a select number of values, the address of the buffer can be modified as such:

~~~
//Setup a Read Buffer
setupReadBuffer(&buffer[0], 8);

//Setup a Write Buffer
setupWriteBuffer(&buffer[4], 4);
~~~

In this configuration, memory writes will occur from the 4th byte to the 7th byte, rather than starting at 0 and going to 3.

#### API Functions

| Function Definition | Description
| ------------------- | --------
| void I2C_BlockData_StoreByte(uint8_t data) | Called by the byte mode driver to handle bytes received. **Do not call this function.**
| uint8_t I2C_BlockData_RequestByte(void) | Called by the byte mode driver to get the next byte to send. **Do not call this function.**
| void I2C_BlockData_onStop(void) | Called by the byte mode driver on an I<sup>2</sup>C stop to adjust or reset the memory indexes. **Do not call this function.**
| void I2C_BlockData_setupReadBuffer(volatile uint8_t* buffer, uint8_t size) | This function sets the read buffer to **SEND** data from the client to the host.
| void I2C_BlockData_setupWriteBuffer(volatile uint8_t* buffer, uint8_t size) | This function sets the write buffer to **RECEIVE** data from the host.  

## Summary  
This example provides a simple bare-metal driver for the I<sup>2</sup>C peripheral to integrate into other projects.
