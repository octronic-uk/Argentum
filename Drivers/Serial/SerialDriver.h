#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "SerialConstants.h"

typedef enum
{
    PARITY_None, 
    PARITY_Odd, 
    PARITY_Even, 
    PARITY_Mark, 
    PARITY_Space
} SerialParity;

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
} SerialBaudRate;

struct SerialPortDescriptor
{
    uint8_t mPortID;
    SerialBaudRate mBaudRate;
    SerialParity mParity;
    uint8_t mLineStatus;
    uint8_t mInterruptID;
    uint8_t mDataBits;
    uint8_t mStopBits;
    uint8_t mInterruptEnable;
    uint8_t mFifoControl;
    uint8_t mModemControl;
    uint8_t mUsingInterrupts;
    volatile uint8_t mCanRead;
    volatile uint8_t mCanWrite;
};
typedef struct SerialPortDescriptor SerialPortDescriptor;

static SerialPortDescriptor SerialPort1_8N1  = 
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
    bool Initialised;
    uint16_t PortAddresses[4];
    uint8_t  Debug;
};
typedef struct SerialDriver SerialDriver;

bool SerialDriver_Constructor(SerialDriver* self);
void SerialDriver_Destrctor(SerialDriver* self);

// Helper Functions
void SerialDriver_SetPortAddressTable(SerialDriver* self);
uint16_t SerialDriver_GetDivisorFromBaudRate(SerialDriver* self,  SerialPortDescriptor* desc);
uint8_t SerialDriver_GetDataBitsMask(SerialDriver* self, SerialPortDescriptor* desc);
uint8_t SerialDriver_GetStopBitsMask(SerialDriver* self, SerialPortDescriptor* desc);
uint8_t SerialDriver_GetParityMask(SerialDriver* self, SerialPortDescriptor* desc);

// Register Manipulation
void SerialDriver_EnableDLAB(SerialDriver* self,  SerialPortDescriptor* desc);
void SerialDriver_DisableDLAB(SerialDriver* self, SerialPortDescriptor* desc);
void SerialDriver_SetBaudRate(SerialDriver* self, SerialPortDescriptor* desc);
void SerialDriver_SetDataBits(SerialDriver* self, SerialPortDescriptor* desc);
void SerialDriver_SetStopBits(SerialDriver* self, SerialPortDescriptor* desc);
void SerialDriver_SetParity(SerialDriver* self,   SerialPortDescriptor* desc);
void SerialDriver_SetInterruptEnableRegister(SerialDriver* self, SerialPortDescriptor* desc);
void SerialDriver_SetModemControlRegister(SerialDriver* self, SerialPortDescriptor*);
void SerialDriver_SetFifoControlRegister(SerialDriver* self, SerialPortDescriptor*);
void SerialDriver_SetLineStatusRegister(SerialDriver* self, SerialPortDescriptor* desc, uint8_t val);

uint8_t SerialDriver_ReadInterruptIDRegister(SerialDriver* self, SerialPortDescriptor* desc);
void SerialDriver_ReadLineStatusRegister(SerialDriver* self, SerialPortDescriptor* desc);

// Interrupts
void SerialDriver_Port1InterruptHandler();
void SerialDriver_SetupInterruptHandlerForPort1(SerialDriver* self );

// Serial IO
uint8_t SerialDriver_Read8b(SerialDriver* self, SerialPortDescriptor* desc);
void SerialDriver_Write8b(SerialDriver* self, SerialPortDescriptor* desc, uint8_t data);
void SerialDriver_WriteString(SerialDriver* self, SerialPortDescriptor* desc, const char* string);

// Debug
void SerialDriver_DebugLineStatus(SerialDriver* self, SerialPortDescriptor* desc);
void SerialDriver_DebugInterruptID(SerialDriver* self, SerialPortDescriptor* desc);

// API
void SerialDriver_SetupPort(SerialDriver* self, SerialPortDescriptor* desc);