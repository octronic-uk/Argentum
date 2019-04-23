#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Drivers/Interrupt/InterruptDriver.h>

/*
    https://wiki.osdev.org/%228042%22_PS/2_Controller
*/

#define PS2_DATA_PORT_RW 0x60
#define PS2_STATUS_REGISTER_R 0x64
#define PS2_COMMAND_REGISTER_W 0x64

// Status Register Flags
#define PS2_STATUS_REG_OUTPUT_BUFFER_READY 0x01
#define PS2_STATUS_REG_INPUT_BUFFERS    0x02
#define PS2_STATUS_REG_SYSTEM_FLAG      0x04
#define PS2_STATUS_REG_INPUT_IS_CMD     0x08
#define PS2_STATUS_REG_KEYBOARD_ENABLED 0x10
#define PS2_STATUS_REG_TX_TIMEOUT       0x20
#define PS2_STATUS_REG_RX_TIMEOUT       0x40
#define PS2_STATUS_REG_PARITY_ERROR     0x80

// PS/2 Commands
#define PS2_CMD_READ_RAM_BYTE_ZERO  0x20
#define PS2_CMD_WRITE_RAM_BYTE_ZERO 0x60
#define PS2_CMD_DISABLE_SECOND_PORT 0xA7
#define PS2_CMD_ENABLE_SECOND_PORT  0xA8
#define PS2_CMD_TEST_SECOND_PORT    0xA9
#define PS2_CMD_TEST_CONTROLLER 0xAA
#define PS2_CMD_TEST_FIRST_PORT     0xAB
#define PS2_CMD_DISABLE_FIRST_PORT  0xAD
#define PS2_CMD_ENABLE_FIRST_PORT   0xAE
#define PS2_CMD_READ_CTRL_OUT_PORT  0xD1
#define PS2_CMD_WRITE_CTRL_OUT_PORT 0xD1
#define PS2_CMD_WRITE_TO_SECOND_PORT 0xD4

// Configuration Byte
#define PS2_CONFIG_FIRST_INTERRUPT    0x01
#define PS2_CONFIG_SECOND_INTERRUPT   0x02
#define PS2_CONFIG_SYSTEM_FLAG            0x04
#define PS2_CONFIG_FIRST_CLOCK        0x10
#define PS2_CONFIG_SECOND_CLOCK       0x20
#define PS2_CONFIG_FIRST_TRANSLATION  0x40

// Misc
#define PS2_TEST_RESULT_PASSED 0x55
#define PS2_TEST_RESULT_FAILED 0xFC
#define PS2_FIRST_PORT_INTERRUPT 0x01
#define PS2_SECOND_PORT_INTERRUPT 0x02
#define PS2_FIRST_PORT_IDT 0x21
#define PS2_SECOND_PORT_IDT 0x2C

#define PS2_DEVICE_RESET 0xFF
#define PS2_DEVICE_RESET_SUCCESS 0xFA
#define PS2_DEVICE_RESET_FAILURE 0xFC

#define PS2_KEY_UP 0x48
#define PS2_KEY_DOWN 0x50

#define PS2_TIMEOUT  10 
#define PS2_WAIT_FOR 50
#define PS2_RESPONSE_BUFFER_SIZE 32

enum _Ps2KeyboardNextByte
{
    Status,
    Scancode
} ;

struct _Ps2KeyboardEvent
{
	uint8_t mKeycode;
	uint8_t mStatus;
};

enum _Ps2ResponseType
{
    Data,
    Command
};

struct _Ps2Response
{
    enum _Ps2ResponseType Type;
    uint8_t Data;
};

struct PS2Driver
{
    struct Interrupt_DescriptorTableEntry FirstPort_IDT_Entry;
    struct Interrupt_DescriptorTableEntry SecondPort_IDT_Entry;
    enum _Ps2KeyboardNextByte KeyboardNextByteExpected;
    bool Debug;
    bool SecondPortExists;
    struct _Ps2Response ResponseBuffer[PS2_RESPONSE_BUFFER_SIZE];
    uint8_t ResponseBufferIndex;
};

bool PS2Driver_Constructor(struct PS2Driver* self);
void PS2Driver_Destructor(struct PS2Driver* self);

void PS2Driver_SetDebug(struct PS2Driver* self, uint8_t debug);

// IO Function Abstractions
void    PS2Driver_FlushDataBuffer(struct PS2Driver* self);
uint8_t PS2Driver_ReadDataPort(struct PS2Driver* self);
void    PS2Driver_WriteDataPort(struct PS2Driver* self, uint8_t data);
uint8_t PS2Driver_ReadStatusRegister(struct PS2Driver* self);
void    PS2Driver_WriteCommandRegister(struct PS2Driver* self, uint8_t cmd);

// PS/2 Controller Commands
uint8_t PS2Driver_CMD_TestController(struct PS2Driver* self);
uint8_t PS2Driver_CMD_TestFirstPort(struct PS2Driver* self);
uint8_t PS2Driver_CMD_TestSecondPort(struct PS2Driver* self);

void PS2Driver_CMD_EnableFirstPort(struct PS2Driver* self);
void PS2Driver_CMD_DisableFirstPort(struct PS2Driver* self);

void PS2Driver_CMD_EnableSecondPort(struct PS2Driver* self);
void PS2Driver_CMD_DisableSecondPort(struct PS2Driver* self);

void PS2Driver_WriteConfigurationByte(struct PS2Driver* self);

// Device Commands
void PS2Driver_DeviceCMD_ResetFirstPort(struct PS2Driver* self);
void PS2Driver_DeviceCMD_ResetSecondPort(struct PS2Driver* self);

// Interrupt Functions
bool PS2Driver_EnableInterrupts(struct PS2Driver* self);
void PS2Driver_SetupInterruptHandlers(struct PS2Driver* self);

void PS2Driver_FirstPortInterruptHandler();
void PS2Driver_SecondPortInterruptHandler();

void PS2Driver_WaitForKeyPress(const char* msg);