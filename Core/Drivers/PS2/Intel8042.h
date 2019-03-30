#pragma once

#include <stdint.h>

/*
    https://wiki.osdev.org/%228042%22_PS/2_Controller
*/

#define I8042_DATA_PORT_RW 0x60
#define I8042_STATUS_REGISTER_R 0x64
#define I8042_COMMAND_REGISTER_W 0x64

// Status Register Flags
#define I8042_STATUS_REG_OUTPUT_BUFFER  0x01
#define I8042_STATUS_REG_INPUT_BUFFERS  0x02
#define I8042_STATUS_REG_SYSTEM_FLAG    0x04
#define I8042_STATUS_REG_CMD_OR_DATA    0x08
#define I8042_STATUS_REG_TIME_OUT_ERROR 0x40
#define I8042_STATUS_REG_PARITY_ERROR   0x80

// PS/2 Commands
#define I8042_CMD_READ_RAM_BYTE_ZERO      0x20
#define I8042_CMD_WRITE_RAM_BYTE_ZERO     0x60
#define I8042_CMD_DISABLE_SECOND_PS2_PORT 0xA7
#define I8042_CMD_ENABLE_SECOND_PS2_PORT  0xA8
#define I8042_CMD_TEST_SECOND_PS2_PORT    0xA9
#define I8042_CMD_TEST_PS2_CONTROLLER     0xAA
#define I8042_CMD_TEST_FIRST_PS2_PORT     0xAB
#define I8042_CMD_DISABLE_FIRST_PS2_PORT  0xAD
#define I8042_CMD_ENABLE_FIRST_PS2_PORT   0xAE
#define I8042_CMD_READ_CTRL_OUT_PORT      0xD1
#define I8042_CMD_WRITE_CTRL_OUT_PORT     0xD1
#define I8042_CMD_WRITE_TO_SECOND_PS2_PORT   0xD4

// Configuration Byte
#define I8042_CONFIG_FIRST_PS2_INTERRUPT    0x01
#define I8042_CONFIG_SECOND_PS2_INTERRUPT   0x02
#define I8042_CONFIG_SYSTEM_FLAG            0x04
#define I8042_CONFIG_FIRST_PS2_CLOCK        0x10
#define I8042_CONFIG_SECOND_PS2_CLOCK       0x20
#define I8042_CONFIG_FIRST_PS2_TRANSLATION  0x40

// Misc
#define I8042_TEST_RESULT_PASSED 0x55
#define I8042_TEST_RESULT_FAILED 0xFC
#define I8042_FIRST_PORT_INTERRUPT 0x01
#define I8042_SECOND_PORT_INTERRUPT 0x02
#define I8042_FIRST_PORT_IDT 0x21
#define I8042_SECOND_PORT_IDT 0x2C

#define I8042_DEVICE_RESET 0xFF
#define I8042_DEVICE_RESET_SUCCESS 0xFA
#define I8042_DEVICE_RESET_FAILURE 0xFC

#define KEY_UP 0x48
#define KEY_DOWN 0x50
#define KEYBOARD_EVENT_BUFFER_SIZE 256

typedef enum
{
    Status,
    Scancode
} I8042_KeyboardNextByte;

typedef struct
{
	uint8_t mKeycode;
	uint8_t mStatus;
} I8042_KeyboardEvent;

void I8042_Constructor();
void I8042_Destructor();

void I8042_SetDebug(uint8_t debug);

// IO Function Abstractions
uint8_t I8042_ReadDataPort();
void    I8042_WriteDataPort(uint8_t data);
uint8_t I8042_ReadStatusRegister();
void    I8042_WriteCommandRegister(uint8_t cmd);

// PS/2 Controller Commands
uint8_t I8042_CMD_TestController();
uint8_t I8042_CMD_TestFirstPort();
uint8_t I8042_CMD_TestSecondPort();

void I8042_CMD_EnableFirstPort();
void I8042_CMD_DisableFirstPort();

void I8042_CMD_EnableSecondPort();
void I8042_CMD_DisableSecondPort();

void I8042_WriteConfigurationByte();

// Device Commands
void I8042_DeviceCMD_ResetFirstPort();
void I8042_DeviceCMD_ResetSecondPort();

// Interrupt Functions
void I8042_EnableInterrupts();
void I8042_SetupInterruptHandlers();

void I8042_FirstPortInterruptHandler();
void I8042_SecondPortInterruptHandler();


void I8042_WaitForKeyPress();