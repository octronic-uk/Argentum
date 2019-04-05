#pragma once

#include <stdint.h>
#include <stdbool.h>

struct Kernel;

#define SERIAL_PORT_1_ADDRESS 0x03F8
#define SERIAL_PORT_2_ADDRESS 0x02F8
#define SERIAL_PORT_3_ADDRESS 0x03E8
#define SERIAL_PORT_4_ADDRESS 0x02E8

#define SERIAL_DLAB_DIVISOR_LSB_REGISTER_OFFSET 0x00
#define SERIAL_DLAB_DIVISOR_MSB_REGISTER_OFFSET 0x01

#define SERIAL_DATA_REGISTER_OFFSET              0x00
#define SERIAL_INTERRUPT_REGISTER_OFFSET         0x01
#define SERIAL_INTERRUPT_ID_REGISTER_OFFSET 0x02
#define SERIAL_FIFO_CTRL_REGISTER_OFFSET 0x02
#define SERIAL_LINE_CONTROL_REGISTER_OFFSET      0x03
#define SERIAL_MODEM_CONTROL_REGISTER_OFFSET     0x04
#define SERIAL_LINE_STATUS_REGISTER_OFFSET       0x05
#define SERIAL_MODEM_STATUS_REGISTER_OFFSET      0x06
#define SERIAL_SCRATCH_REGISTER_OFFSET           0x07

#define SERIAL_BAUD_DIVISOR_115200 1
#define SERIAL_BAUD_DIVISOR_57600  2
#define SERIAL_BAUD_DIVISOR_38400  3
#define SERIAL_BAUD_DIVISOR_19200  6
#define SERIAL_BAUD_DIVISOR_9600   12
#define SERIAL_BAUD_DIVISOR_4800   24
#define SERIAL_BAUD_DIVISOR_2400   48
#define SERIAL_BAUD_DIVISOR_1200   96 
#define SERIAL_BAUD_DIVISOR_300    384
#define SERIAL_BAUD_DIVISOR_50     2304

#define SERIAL_DLAB_ENABLE_MASK  0x80
#define SERIAL_DLAB_DISABLE_MASK 0x7F

#define SERIAL_DATA_BITS_CLEAR_MASK 0xFC
#define SERIAL_DATA_BITS_5_MASK     0x00
#define SERIAL_DATA_BITS_6_MASK     0x01
#define SERIAL_DATA_BITS_7_MASK     0x02
#define SERIAL_DATA_BITS_8_MASK     0x03

#define SERIAL_STOP_BITS_CLEAR_MASK 0xFB
#define SERIAL_STOP_BITS_1_MASK     0x00
#define SERIAL_STOP_BITS_2_MASK     0x04

#define SERIAL_PARITY_CLEAR_MASK 0xC7
#define SERIAL_PARITY_NONE_MASK  0x00
#define SERIAL_PARITY_ODD_MASK   0x08
#define SERIAL_PARITY_EVEN_MASK  0x18
#define SERIAL_PARITY_MARK_MASK  0x28
#define SERIAL_PARITY_SPACE_MASK 0x38

#define SERIAL_STATUS_DATA_READY                 0x01
#define SERIAL_STATUS_OVERRUN_ERROR              0x02
#define SERIAL_STATUS_PARITY_ERROR               0x04
#define SERIAL_STATUS_FRAMING_ERROR              0x08
#define SERIAL_STATUS_BREAK_INDICATOR            0x10
#define SERIAL_STATUS_TRANSMITTER_HOLD_REG_EMPTY 0x20
#define SERIAL_STATUS_TRANSMITTER_EMPTY          0x40
#define SERIAL_STATUS_IMPENDING_ERROR            0x80

#define SERIAL_IIR_INTERRUPT_PENDING    0x00
#define SERIAL_IIR_NO_INTERRUPT_PENDING 0x01
#define SERIAL_IIR_MODEM_STATUS_CHANGE  0x00
#define SERIAL_IIR_THRE                 0x02
#define SERIAL_IIR_RX_DATA_AVAILABLE    0x04
#define SERIAL_IIR_LINE_STATUS_CHANGE   0x06
#define SERIAL_IIR_CHAR_TIMEOUT         0x0C

typedef enum
{
    PARITY_None, 
    PARITY_Odd, 
    PARITY_Even, 
    PARITY_Mark, 
    PARITY_Space
} Serial_Parity;

typedef enum
{
    BAUD_115200,
    BAUD_57600,
    BAUD_38400,
    BAUD_19200,
    BAUD_9600,
    BAUD_4800,
    BAUD_2400,
    BAUD_1200,
    BAUD_300,
    BAUD_50
} Serial_BaudRate;

struct Serial_PortDescriptor
{
    uint8_t mPortID;
    Serial_BaudRate mBaudRate;
    Serial_Parity mParity;
    uint8_t mLineStatus;
    uint8_t mInterruptID;
    uint8_t mDataBits;
    uint8_t mStopBits;
    uint8_t mInterruptEnable;
    uint8_t mFifoControl;
    uint8_t mModemControl;
    uint8_t mUsingInterrupts;
    uint8_t mCanRead;
    uint8_t mCanWrite;
} ;

static struct Serial_PortDescriptor Serial_Port1_8N1  = 
{
    .mPortID = 1,
    .mCanRead = 0,
    .mCanWrite = 1,
    .mBaudRate = BAUD_115200,
    .mDataBits = 8,
    .mParity = PARITY_None,
    .mStopBits = 1,
    .mInterruptEnable = 0x0F,
    .mFifoControl = 0xC7,
    .mModemControl = 0x0B,
    .mLineStatus = 0,
    .mInterruptID = 0,
};

struct SerialDriver
{
    struct Kernel* Kernel;
    uint16_t PortAddresses[4];
    uint8_t  Debug;
};

bool SerialDriver_Constructor(struct SerialDriver* self, struct Kernel* kernel);
void SerialDriver_Destrctor(struct SerialDriver* self);

// Helper Functions
void SerialDriver_SetPortAddressTable(struct SerialDriver* self);
uint16_t SerialDriver_GetDivisorFromBaudRate(struct SerialDriver* self,  struct Serial_PortDescriptor* desc);
uint8_t SerialDriver_GetDataBitsMask(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
uint8_t SerialDriver_GetStopBitsMask(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
uint8_t SerialDriver_GetParityMask(struct SerialDriver* self, struct Serial_PortDescriptor* desc);

// Register Manipulation
void SerialDriver_EnableDLAB(struct SerialDriver* self,  struct Serial_PortDescriptor* desc);
void SerialDriver_DisableDLAB(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
void SerialDriver_SetBaudRate(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
void SerialDriver_SetDataBits(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
void SerialDriver_SetStopBits(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
void SerialDriver_SetParity(struct SerialDriver* self,   struct Serial_PortDescriptor* desc);
void SerialDriver_SetInterruptEnableRegister(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
void SerialDriver_SetModemControlRegister(struct SerialDriver* self, struct Serial_PortDescriptor*);
void SerialDriver_SetFifoControlRegister(struct SerialDriver* self, struct Serial_PortDescriptor*);
void SerialDriver_SetLineStatusRegister(struct SerialDriver* self,  struct Serial_PortDescriptor* desc, uint8_t val);

uint8_t SerialDriver_ReadInterruptIDRegister(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
void SerialDriver_ReadLineStatusRegister(struct SerialDriver* self, struct Serial_PortDescriptor* desc);

// Interrupts
void SerialDriver_Port1InterruptHandler();
void SerialDriver_SetupInterruptHandlerForPort1(struct SerialDriver* self );

// Serial IO
uint8_t SerialDriver_Read8b(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
void SerialDriver_Write8b(struct SerialDriver* self, struct Serial_PortDescriptor* desc, uint8_t data);
void SerialDriver_WriteString(struct SerialDriver* self, struct Serial_PortDescriptor* desc, const char* string);

// Debug
void SerialDriver_DebugLineStatus(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
void SerialDriver_DebugInterruptID(struct SerialDriver* self, struct Serial_PortDescriptor* desc);

// API
void SerialDriver_SetupPort(struct SerialDriver* self, struct Serial_PortDescriptor* desc);
void SerialDriver_TestPort1(struct SerialDriver* self);