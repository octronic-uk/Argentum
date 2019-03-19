#pragma once

#include <LibC/include/stdint.h>

#define SERIAL_BUFFER_SIZE 64

#define SERIAL_PORT_1_ADDRESS 0x03F8
#define SERIAL_PORT_2_ADDRESS 0x02F8
#define SERIAL_PORT_3_ADDRESS 0x03E8
#define SERIAL_PORT_4_ADDRESS 0x02E8

#define SERIAL_PORT_1_IDT_INDEX 0x24
#define SERIAL_PORT_2_IDT_INDEX 0x23

#define SERIAL_DLAB_DIVISOR_LSB_REGISTER_OFFSET 0x00
#define SERIAL_DLAB_DIVISOR_MSB_REGISTER_OFFSET 0x01

#define SERIAL_DATA_REGISTER_OFFSET              0x00
#define SERIAL_INTERRUPT_REGISTER_OFFSET         0x01
#define SERIAL_INTERRUPT_ID_FIFO_REGISTER_OFFSET 0x02
#define SERIAL_LINE_CONTROL_REGISTER_OFFSET      0x03
#define SERIAL_MODEM_CONTROL_REGISTER_OFFSET     0x04
#define SERIAL_LINE_STATUS_REGISTER_OFFSET       0x05
#define SERIAL_MODEM_STATUS_REGISTER_OFFSET      0x06
#define SERIAL_SCRATCH_REGISTER_OFFSET           0x07

#define SERIAL_BAUD_DIVISOR_115200 0x01
#define SERIAL_BAUD_DIVISOR_57600  0x02
#define SERIAL_BAUD_DIVISOR_38400  0x03
#define SERIAL_BAUD_DIVISOR_28800  0x04
#define SERIAL_BAUD_DIVISOR_23040  0x05
#define SERIAL_BAUD_DIVISOR_19200  0x06
#define SERIAL_BAUD_DIVISOR_14400  0x08
#define SERIAL_BAUD_DIVISOR_12800  0x09
#define SERIAL_BAUD_DIVISOR_11520  0x0A
#define SERIAL_BAUD_DIVISOR_9600   0x0C

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

#define SERIAL_INTERRUPT_DATA_AVAILABLE_MASK    0x01
#define SERIAL_INTERRUPT_TRANSMITTER_EMPTY_MASK 0x02
#define SERIAL_INTERRUPT_BREAK_ERROR_MASK       0x04
#define SERIAL_INTERRUPT_STATUS_CHANGE_MASK     0x08

#define SERIAL_STATUS_DATA_READY                 0x01
#define SERIAL_STATUS_OVERRUN_ERROR              0x02
#define SERIAL_STATUS_PARITY_ERROR               0x04
#define SERIAL_STATUS_FRAMING_ERROR              0x08
#define SERIAL_STATUS_BREAK_INDICATOR            0x10
#define SERIAL_STATUS_TRANSMITTER_HOLD_REG_EMPTY 0x20
#define SERIAL_STATUS_TRANSMITTER_EMPTY          0x40
#define SERIAL_STATUS_IMPENDING_ERROR            0x80

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
    BAUD_28800,
    BAUD_23040,
    BAUD_19200,
    BAUD_14400,
    BAUD_12800,
    BAUD_11520,
    BAUD_9600
} Serial_BaudRate;

typedef struct 
{
    uint8_t mPortID;
    Serial_BaudRate mBaudRate;
    Serial_Parity mParity;
    uint8_t mStatus;
    uint8_t mDataBits;
    uint8_t mStopBits;
    uint8_t mInterruptEnable;
    uint8_t mIDFifo;
    uint8_t mModemControl;

    uint8_t mInputBuffer[SERIAL_BUFFER_SIZE];
    uint8_t mInputBufferReadIndex;
    uint8_t mInputBufferWriteIndex;

    uint8_t mOutputBuffer[SERIAL_BUFFER_SIZE];
    uint8_t mOutputBufferReadIndex;
    uint8_t mOutputBufferWriteIndex;
    uint8_t mOutputBytesToWrite;
} Serial_PortDescriptor;

static Serial_PortDescriptor Serial_Port1_8N1  = 
{
    .mPortID = 1,
    .mBaudRate = BAUD_9600,
    .mDataBits = 8,
    .mParity = PARITY_None,
    .mStopBits = 1,
    .mInterruptEnable = 0x00,
    .mIDFifo = 0xC7,

    .mInputBuffer = {0},
    .mInputBufferReadIndex = 0,
    .mInputBufferWriteIndex = 0,

    .mOutputBuffer = {0},
    .mOutputBufferReadIndex = 0,
    .mOutputBufferWriteIndex = 0,
    .mOutputBytesToWrite = 0
};

void Serial_Constructor();
void Serial_Destrctor();

extern void Serial_Port1InterruptHandlerASM();
void Serial_Port1InterruptHandler();
void Serial_SetupInterruptForPort1();
void Serial_TestPort1();

void Serial_SetPortAddressTable();

void Serial_EnableDLAB(Serial_PortDescriptor* desc);
void Serial_DisableDLAB(Serial_PortDescriptor* desc);

void Serial_SetBaudRate(Serial_PortDescriptor* desc);
uint8_t Serial_GetDataBitsMask(Serial_PortDescriptor* desc);
uint16_t Serial_GetDivisorFromBaudRate(Serial_BaudRate baudRate);

void Serial_SetDataBits(Serial_PortDescriptor* desc);

void Serial_SetStopBits(Serial_PortDescriptor* desc);
uint8_t Serial_GetStopBitsMask(Serial_PortDescriptor* desc);

void Serial_SetParity(Serial_PortDescriptor* desc);
uint8_t Serial_GetParityMask(Serial_PortDescriptor* desc);

void Serial_SetInterruptEnableRegister(Serial_PortDescriptor* desc);

uint8_t Serial_Read8b(Serial_PortDescriptor* desc);

void Serial_Write8b(Serial_PortDescriptor* des);
uint8_t Serial_WriteString(Serial_PortDescriptor* desc, const char* string, uint32_t len);

void Serial_SetupPort(Serial_PortDescriptor* desc);

void Serial_CheckStatus(Serial_PortDescriptor* desc);
void Serial_ReadLineStatusRegister(Serial_PortDescriptor* desc);
void Serial_SetIDFIFORegister(Serial_PortDescriptor*);
void Serial_SetModemControlRegister(Serial_PortDescriptor*);