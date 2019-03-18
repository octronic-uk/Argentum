#pragma once

#include <LibC/include/stdint.h>

#define SERIAL_PORT_1_ADDRESS 0x03F8
#define SERIAL_PORT_2_ADDRESS 0x02F8
#define SERIAL_PORT_3_ADDRESS 0x03E8
#define SERIAL_PORT_4_ADDRESS 0x02E8

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

#define SERIAL_DLAB_ENABLE_MASK  0x08
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
    None, Odd, Even, Mark, Space
} SerialParity;

typedef struct 
{
    uint8_t mPortID;
    uint16_t mBaudRate;
    uint8_t mDataBits;
    uint8_t mStopBits;
    SerialParity mParity;
    uint8_t mInterrupts;
} Serial_PortDescriptor;

static Serial_PortDescriptor Serial_Port1_8N1  = {
    .mPortID = 1,
    .mBaudRate = 9600,
    .mDataBits = 8,
    .mParity = None,
    .mStopBits = 1,
    .mInterrupts = 0
};

void Serial_Constructor();
void Serial_Destrctor();


void Serial_SetPortAddressTable();

void Serial_EnableDLAB(Serial_PortDescriptor desc);
void Serial_DisableDLAB(Serial_PortDescriptor desc);

void Serial_SetBaudRate(Serial_PortDescriptor desc);
uint8_t Serial_GetDataBitsMask(Serial_PortDescriptor desc);

void Serial_SetDataBits(Serial_PortDescriptor desc);

void Serial_SetStopBits(Serial_PortDescriptor desc);
uint8_t Serial_GetStopBitsMask(Serial_PortDescriptor desc);

void Serial_SetParity(Serial_PortDescriptor desc);
uint8_t Serial_GetParityMask(Serial_PortDescriptor desc);

void Serial_SetInterruptRegister(Serial_PortDescriptor desc);

uint8_t Serial_IsDataReady(Serial_PortDescriptor desc);
uint8_t Serial_Read8b(Serial_PortDescriptor desc);

uint8_t Serial_IsTransmitEmpty(Serial_PortDescriptor desc);
void Serial_Write8b(Serial_PortDescriptor des, uint8_t data);

void Serial_SetupPort(Serial_PortDescriptor desc);


void Serial_WriteString(Serial_PortDescriptor desc, const char* string, uint32_t len);