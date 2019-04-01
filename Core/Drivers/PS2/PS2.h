#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Drivers/Interrupt/Interrupt.h>

/*
    https://wiki.osdev.org/%228042%22_PS/2_Controller
*/


#define PS2_DATA_PORT_RW 0x60
#define PS2_STATUS_REGISTER_R 0x64
#define PS2_COMMAND_REGISTER_W 0x64

// Status Register Flags
#define PS2_STATUS_REG_OUTPUT_BUFFER  0x01
#define PS2_STATUS_REG_INPUT_BUFFERS  0x02
#define PS2_STATUS_REG_SYSTEM_FLAG    0x04
#define PS2_STATUS_REG_CMD_OR_DATA    0x08
#define PS2_STATUS_REG_TIME_OUT_ERROR 0x40
#define PS2_STATUS_REG_PARITY_ERROR   0x80

// PS/2 Commands
#define PS2_CMD_READ_RAM_BYTE_ZERO      0x20
#define PS2_CMD_WRITE_RAM_BYTE_ZERO     0x60
#define PS2_CMD_DISABLE_SECOND_PS2_PORT 0xA7
#define PS2_CMD_ENABLE_SECOND_PS2_PORT  0xA8
#define PS2_CMD_TEST_SECOND_PS2_PORT    0xA9
#define PS2_CMD_TEST_PS2_CONTROLLER     0xAA
#define PS2_CMD_TEST_FIRST_PS2_PORT     0xAB
#define PS2_CMD_DISABLE_FIRST_PS2_PORT  0xAD
#define PS2_CMD_ENABLE_FIRST_PS2_PORT   0xAE
#define PS2_CMD_READ_CTRL_OUT_PORT      0xD1
#define PS2_CMD_WRITE_CTRL_OUT_PORT     0xD1
#define PS2_CMD_WRITE_TO_SECOND_PS2_PORT   0xD4

// Configuration Byte
#define PS2_CONFIG_FIRST_PS2_INTERRUPT    0x01
#define PS2_CONFIG_SECOND_PS2_INTERRUPT   0x02
#define PS2_CONFIG_SYSTEM_FLAG            0x04
#define PS2_CONFIG_FIRST_PS2_CLOCK        0x10
#define PS2_CONFIG_SECOND_PS2_CLOCK       0x20
#define PS2_CONFIG_FIRST_PS2_TRANSLATION  0x40

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

#define KEY_UP 0x48
#define KEY_DOWN 0x50
#define KEYBOARD_EVENT_BUFFER_SIZE 256

enum PS2_KeyboardNextByte
{
    Status,
    Scancode
} ;

struct PS2_KeyboardEvent
{
	uint8_t mKeycode;
	uint8_t mStatus;
};

struct PS2
{
    struct Interrupt_DescriptorTableEntry FirstPort_IDT_Entry;
    struct Interrupt_DescriptorTableEntry SecondPort_IDT_Entry;
    enum PS2_KeyboardNextByte KeyboardNextByteExpected;
    struct Interrupt* Interrupt;
    bool Debug;
    bool SecondPortExists;
    volatile bool WaitingForKeyPress;
};

void PS2_Constructor(struct PS2* self, struct Interrupt* interrupt);
void PS2_Destructor(struct PS2* self);

void PS2_SetDebug(struct PS2* self, uint8_t debug);

// IO Function Abstractions
uint8_t PS2_ReadDataPort(struct PS2* self);
void    PS2_WriteDataPort(struct PS2* self, uint8_t data);
uint8_t PS2_ReadStatusRegister(struct PS2* self);
void    PS2_WriteCommandRegister(struct PS2* self, uint8_t cmd);

// PS/2 Controller Commands
uint8_t PS2_CMD_TestController(struct PS2* self);
uint8_t PS2_CMD_TestFirstPort(struct PS2* self);
uint8_t PS2_CMD_TestSecondPort(struct PS2* self);

void PS2_CMD_EnableFirstPort(struct PS2* self);
void PS2_CMD_DisableFirstPort(struct PS2* self);

void PS2_CMD_EnableSecondPort(struct PS2* self);
void PS2_CMD_DisableSecondPort(struct PS2* self);

void PS2_WriteConfigurationByte(struct PS2* self);

// Device Commands
void PS2_DeviceCMD_ResetFirstPort(struct PS2* self);
void PS2_DeviceCMD_ResetSecondPort(struct PS2* self);

// Interrupt Functions
void PS2_EnableInterrupts(struct PS2* self);
void PS2_SetupInterruptHandlers(struct PS2* self);

void PS2_FirstPortInterruptHandler();
void PS2_SecondPortInterruptHandler();


void PS2_WaitForKeyPress(struct PS2* self);