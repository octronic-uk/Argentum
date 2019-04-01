#pragma once

#include <stdint.h>

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

typedef struct 
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
} Serial_PortDescriptor;

static volatile Serial_PortDescriptor Serial_Port1_8N1  = 
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

struct Serial
{
    struct Interrupt* Interrupt;
    uint16_t PortAddresses[4];
    uint8_t  Debug;
};

void Serial_Constructor(struct Serial* self, struct Interrupt* interrupt);
void Serial_Destrctor(struct Serial* self);

// Helper Functions
void Serial_SetPortAddressTable(struct Serial* self);
uint16_t Serial_GetDivisorFromBaudRate(struct Serial* self, volatile Serial_PortDescriptor* desc);
uint8_t Serial_GetDataBitsMask(struct Serial* self, volatile Serial_PortDescriptor* desc);
uint8_t Serial_GetStopBitsMask(struct Serial* self, volatile Serial_PortDescriptor* desc);
uint8_t Serial_GetParityMask(struct Serial* self, volatile Serial_PortDescriptor* desc);

// Register Manipulation
void Serial_EnableDLAB(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_DisableDLAB(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_SetBaudRate(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_SetDataBits(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_SetStopBits(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_SetParity(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_SetInterruptEnableRegister(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_SetModemControlRegister(struct Serial* self, volatile Serial_PortDescriptor*);
void Serial_SetFifoControlRegister(struct Serial* self, volatile Serial_PortDescriptor*);
void Serial_SetLineStatusRegister(struct Serial* self, volatile Serial_PortDescriptor* desc, uint8_t val);

uint8_t Serial_ReadInterruptIDRegister(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_ReadLineStatusRegister(struct Serial* self, volatile Serial_PortDescriptor* desc);

// Interrupts
void Serial_Port1InterruptHandler();
void Serial_SetupInterruptForPort1(struct Serial* self );

// Serial IO
uint8_t Serial_Read8b(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_Write8b(struct Serial* self, volatile Serial_PortDescriptor* desc, uint8_t data);
void Serial_WriteString(struct Serial* self, volatile Serial_PortDescriptor* desc, const char* string);

// Debug
void Serial_DebugLineStatus(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_DebugInterruptID(struct Serial* self, volatile Serial_PortDescriptor* desc);

// API
void Serial_SetupPort(struct Serial* self, volatile Serial_PortDescriptor* desc);
void Serial_TestPort1(struct Serial* self);