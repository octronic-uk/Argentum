#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "ScancodeParser.h"
#include "MouseState.h"
#include "Event.h"

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

#define PS2_CMD_READ_CTRL_OUT_PORT  0xD0
#define PS2_CMD_WRITE_CTRL_OUT_PORT 0xD1
#define PS2_CMD_WRITE_CTRL_OUT_BUFFER 0xD2
#define PS2_CMD_WRITE_TO_SECOND_OUT_BUFFER  0xD3
#define PS2_CMD_WRITE_TO_SECOND_IN_BUFFER 0xD4

#define PS2_CMD_IDENTIFY         0xF2
#define PS2_CMD_DISABLE_SCANNING 0xF5

#define PS2_CMD_RESPONSE_ACK 0xFA

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

#define PS2_TIMEOUT  100 
#define PS2_WAIT_FOR 50
#define PS2_RESPONSE_BUFFER_SIZE 32

// Mouse

#define PS2_MOUSE_CMD_RESET                  0xFF // Reset
#define PS2_MOUSE_CMD_RESEND                 0xFE // Resend
#define PS2_MOUSE_CMD_SET_DEFAULTS           0xF6 // Set Defaults
#define PS2_MOUSE_CMD_DISABLE_DATA_REPORTING 0xF5 // Disable Data Reporting
#define PS2_MOUSE_CMD_ENABLE_DATA_REPORTING  0xF4 // Enable Data Reporting
#define PS2_MOUSE_CMD_SET_SAMPLE_RATE        0xF3 // Set Sample Rate, valid values are 10, 20, 40, 60, 80, 100, and 200.
#define PS2_MOUSE_CMD_GET_DEVICE_IDE         0xF2 // Get Device ID. See Detecting PS/2 Device Types for the response bytes.
#define PS2_MOUSE_CMD_SET_REMOTE_MODE        0xF0 // Set Remote Mode
#define PS2_MOUSE_CMD_SET_WRAP_MODE          0xEE // Set Wrap Mode
#define PS2_MOUSE_CMD_RESET_WRAP_MODE        0xEC // Reset Wrap Mode
#define PS2_MOUSE_CMD_READ_DATA              0xEB // Read Data
#define PS2_MOUSE_CMD_SET_STREAM_MODE        0xEA // Set Stream Mode
#define PS2_MOUSE_CMD_GET_STATUS             0xE9 // Status Request
#define PS2_MOUSE_CMD_SET_RESOLUTION         0xE8 // Resolution: 1 count/mm, 2 count/mm, 4 count/mm, 8 count/mm
#define PS2_MOUSE_CMD_SET_SCALING            0xE6 // 1 or 2	Set Scaling

struct PS2Driver
{
    bool Debug;
    bool SecondPortExists;
    struct ScancodeParser ScancodeParser;
    struct MouseState MouseState;
};

typedef struct PS2Driver PS2Driver;

bool PS2Driver_Constructor(PS2Driver* self);
void PS2Driver_Destructor(PS2Driver* self);

// IO Function Abstractions
void    PS2Driver_FlushDataBuffer(PS2Driver* self);
uint8_t PS2Driver_ReadDataPort(PS2Driver* self);
void    PS2Driver_WriteDataPort(PS2Driver* self, uint8_t data);
uint8_t PS2Driver_ReadStatusRegister(PS2Driver* self);
void    PS2Driver_WriteCommandRegister(PS2Driver* self, uint8_t cmd);

// PS/2 Controller Commands
uint8_t PS2Driver_CMD_TestController(PS2Driver* self);
uint8_t PS2Driver_CMD_TestFirstPort(PS2Driver* self);
uint8_t PS2Driver_CMD_TestSecondPort(PS2Driver* self);

void PS2Driver_CMD_EnableFirstPort(PS2Driver* self);
void PS2Driver_CMD_DisableFirstPort(PS2Driver* self);

void PS2Driver_CMD_EnableSecondPort(PS2Driver* self);
void PS2Driver_CMD_DisableSecondPort(PS2Driver* self);

void PS2Driver_WriteInitialConfigurationByte(PS2Driver* self);

// Device Commands
void PS2Driver_DeviceCMD_ResetFirstPort(PS2Driver* self);
void PS2Driver_DeviceCMD_ResetSecondPort(PS2Driver* self);

void PS2Driver_IdentifyPort1(PS2Driver* self);
void PS2Driver_IdentifyPort2(PS2Driver* self);

bool PS2Driver_MouseEnable(PS2Driver* self);
uint8_t PS2Driver_ReadFromSecondPort(PS2Driver* self);
void PS2Driver_WriteToSecondPort(PS2Driver* self, uint8_t data);


// Interrupt Functions
bool PS2Driver_EnableInterrupts(PS2Driver* self);
void PS2Driver_SetupInterruptHandlers(PS2Driver* self);

void PS2Driver_FirstPortInterruptHandler();
void PS2Driver_SecondPortInterruptHandler();

char PS2Driver_WaitForChar();
char PS2Driver_ScancodeToChar(uint8_t scancode);

void PS2Driver_SetKeyboardEventCallback(PS2Driver* self, void(*callback)(KeyboardEvent));
void PS2Driver_SetMouseEventCallback(PS2Driver* self, void(*callback)(MouseEvent));