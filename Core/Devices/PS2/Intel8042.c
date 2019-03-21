#include "Intel8042.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <Devices/IO/IO.h>
#include <Devices/Interrupt/Interrupt.h>

Interrupt_DescriptorTableEntry I8042_FirstPort_IDT_Entry;
Interrupt_DescriptorTableEntry I8042_SecondPort_IDT_Entry;

uint8_t I8042_Debug = 1;
uint8_t I8042_SecondPortExists = 0;

void I8042_Constructor()
{
    printf("I8042: Constructing\n");
    memset(&I8042_FirstPort_IDT_Entry, 0, sizeof(Interrupt_DescriptorTableEntry));
    memset(&I8042_SecondPort_IDT_Entry, 0, sizeof(Interrupt_DescriptorTableEntry));

    I8042_SetupInterruptHandlers();
    I8042_CMD_DisableFirstPort();
    I8042_CMD_DisableSecondPort();
    I8042_ReadDataPort(); // to flush the output buffer
    I8042_WriteConfigurationByte();
    if (I8042_CMD_TestController())
    {
        if (I8042_CMD_TestFirstPort())
        {
            I8042_CMD_EnableFirstPort();
        }
        if (I8042_SecondPortExists)
        {
            if(I8042_CMD_TestSecondPort())
            {
                I8042_CMD_EnableSecondPort();
            }
        }
    }
    I8042_SetupInterruptHandlers();
}

void I8042_Destructor()
{
    printf("I8042: Destructing\n");
}

// IO Function Abstractions ====================================================
uint8_t I8042_ReadDataPort()
{
    return IO_ReadPort8b(I8042_DATA_PORT_RW);
}

void I8042_WriteDataPort(uint8_t data)
{
    IO_WritePort8b(I8042_DATA_PORT_RW, data);
}

uint8_t I8042_ReadStatusRegister()
{
    return IO_ReadPort8b(I8042_STATUS_REGISTER_R);
}

void I8042_WriteCommandRegister(uint8_t cmd)
{
    IO_WritePort8b(I8042_COMMAND_REGISTER_W, cmd);
}

// PS/2 Controller Commands ====================================================

void I8042_CMD_EnableFirstPort()
{
if (I8042_Debug)
    {
        printf("I8042: Enabling First Port\n");
    }
    I8042_WriteCommandRegister(I8042_CMD_ENABLE_FIRST_PS2_PORT);
}

void I8042_CMD_DisableFirstPort()
{
if (I8042_Debug)
    {
        printf("I8042: Disabling First Port\n");
    }
    I8042_WriteCommandRegister(I8042_CMD_DISABLE_FIRST_PS2_PORT);
}

void I8042_CMD_EnableSecondPort()
{
if (I8042_Debug)
    {
        printf("I8042: Enabling Second Port\n");
    }
    I8042_WriteCommandRegister(I8042_CMD_ENABLE_SECOND_PS2_PORT);
}

void I8042_CMD_DisableSecondPort()
{
    if (I8042_Debug)
    {
        printf("I8042: Disabling Second Port\n");
    }
    I8042_WriteCommandRegister(I8042_CMD_DISABLE_SECOND_PS2_PORT);
}

uint8_t I8042_CMD_TestController()
{
    // Write command
    if (I8042_Debug) printf("I8042: Testing PS/2 Controller\n");
    I8042_WriteCommandRegister(I8042_CMD_TEST_PS2_CONTROLLER);
    // Wait for result to appear
    while ((I8042_ReadStatusRegister() & I8042_STATUS_REG_OUTPUT_BUFFER) == 0)
    {
        usleep(20);
    }
    // Read Result
    uint8_t result = I8042_ReadDataPort();
    // Test Result
    if ((result & I8042_TEST_RESULT_PASSED) == I8042_TEST_RESULT_PASSED) 
    {
        if (I8042_Debug) printf("I8042: Testing PS/2 Controller PASSED\n");
        return 1;
    }
    else if ((result & I8042_TEST_RESULT_FAILED) == I8042_TEST_RESULT_FAILED) 
    {
        if (I8042_Debug) printf("I8042: Testing PS/2 Controller FAILED\n");
        return 0;
    }
    else 
    {
        if (I8042_Debug) printf("I8042: Testing PS/2 Controller FLOPPED :/\n");
        return 0;
    }
}

uint8_t I8042_CMD_TestFirstPort()
{
    // Write command
    if (I8042_Debug) printf("I8042: Testing First Port\n");
    I8042_WriteCommandRegister(I8042_CMD_TEST_FIRST_PS2_PORT);
    // Wait for result to appear
    while ((I8042_ReadStatusRegister() & I8042_STATUS_REG_OUTPUT_BUFFER) == 0)
    {
        usleep(20);
    }
    // Read Result
    uint8_t result = I8042_ReadDataPort();
    // Test Result
    if (I8042_Debug) 
    {
        if (result == 0) 
        {
            printf("I8042: Testing First Port PASSED\n");
            return 1;
        }
        else 
        {
            printf("I8042: Testing First Port FAILED with 0x%x\n",result);
            return 0;
        }
    }
}

uint8_t I8042_CMD_TestSecondPort()
{
    // Write command
    if (I8042_Debug) printf("I8042: Testing First Port\n");
    I8042_WriteCommandRegister(I8042_CMD_TEST_SECOND_PS2_PORT);
    // Wait for result to appear
    while ((I8042_ReadStatusRegister() & I8042_STATUS_REG_OUTPUT_BUFFER) == 0)
    {
        usleep(20);
    }
    // Read Result
    uint8_t result = I8042_ReadDataPort();
    // Test Result
    if (I8042_Debug) 
    {
        if (result == 0) 
        {
            printf("I8042: Testing Second Port PASSED\n");
            return 1;
        }
        else 
        {
            printf("I8042: Testing Second Port FAILED with 0x%x\n",result);
            return 0;
        }
    }
}

void I8042_WriteConfigurationByte()
{
    if (I8042_Debug) printf("I8042: Writing config byte\n");
    
    I8042_WriteCommandRegister(I8042_CMD_READ_RAM_BYTE_ZERO);

    while ((I8042_ReadStatusRegister() & I8042_STATUS_REG_CMD_OR_DATA) == 0)
    {
        usleep(20);
    }
    // Read Result
    uint8_t currentConfig = I8042_ReadDataPort(); 

    if (I8042_Debug) printf("I8042: Current config byte 0x%x\n",currentConfig);

    I8042_SecondPortExists = (currentConfig & I8042_CONFIG_SECOND_PS2_CLOCK) != 0;

    currentConfig = currentConfig & 0xDC;

    I8042_WriteCommandRegister(I8042_CMD_WRITE_RAM_BYTE_ZERO);

    while ((I8042_ReadStatusRegister() & I8042_STATUS_REG_CMD_OR_DATA) == 0 )
    {
        usleep(20);
    }

    if (I8042_Debug) printf("I8042: Writing new config byte 0x%x\n",currentConfig);

    I8042_WriteDataPort(currentConfig);

    if (I8042_Debug)
    {
        if (I8042_SecondPortExists) printf("I8042: Second Port found\n");
        else printf("I8042: Second Port NOT found\n");
    }
}

// Interrupt ==================================================================

void I8042_EnableInterrupts()
{
    if (I8042_Debug) printf("I8042: Enabling Interrupts\n");
    
    I8042_WriteCommandRegister(I8042_CMD_READ_RAM_BYTE_ZERO);

    while ((I8042_ReadStatusRegister() & I8042_STATUS_REG_CMD_OR_DATA) == 0)
    {
        usleep(20);
    }
    // Read Result
    uint8_t currentConfig = I8042_ReadDataPort(); 

    if (I8042_Debug) printf("I8042: Current config byte 0x%x\n",currentConfig);

    currentConfig &= I8042_FIRST_PORT_INTERRUPT;

    if (I8042_SecondPortExists)
    {
        currentConfig &= I8042_SECOND_PORT_INTERRUPT;
    }

    I8042_WriteCommandRegister(I8042_CMD_WRITE_RAM_BYTE_ZERO);

    while ((I8042_ReadStatusRegister() & I8042_STATUS_REG_CMD_OR_DATA) == 0 )
    {
        usleep(20);
    }

    if (I8042_Debug) printf("I8042: Writing new config byte 0x%x\n",currentConfig);

    I8042_WriteDataPort(currentConfig);
}

void I8042_SetupInterruptHandlers()
{
    // First Port
    if (I8042_Debug)
    {
	    printf("I8042: Setting first port IDT entry.\n");
	}
	uint32_t first_port_asm = (uint32_t)I8042_FirstPortInterruptHandlerASM;
	I8042_FirstPort_IDT_Entry.mOffsetLowerBits = first_port_asm & 0xffff;
	I8042_FirstPort_IDT_Entry.mOffsetHigherBits = (first_port_asm & 0xffff0000) >> 16;
	I8042_FirstPort_IDT_Entry.mSelector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	I8042_FirstPort_IDT_Entry.mZero = 0;
	I8042_FirstPort_IDT_Entry.mTypeAttribute = INTERRUPT_GATE;
	Interrupt_SetIDTEntry(I8042_FIRST_PORT_IDT,I8042_FirstPort_IDT_Entry);

    // First Port
    if (I8042_SecondPortExists)
    {
        if (I8042_Debug)
        {
            printf("I8042: Setting second port IDT entry.\n");
        }
        uint32_t second_port_asm = (uint32_t)I8042_SecondPortInterruptHandlerASM;
        I8042_SecondPort_IDT_Entry.mOffsetLowerBits = second_port_asm & 0xffff;
        I8042_SecondPort_IDT_Entry.mOffsetHigherBits = (second_port_asm & 0xffff0000) >> 16;
        I8042_SecondPort_IDT_Entry.mSelector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
        I8042_SecondPort_IDT_Entry.mZero = 0;
        I8042_SecondPort_IDT_Entry.mTypeAttribute = INTERRUPT_GATE;
        Interrupt_SetIDTEntry(I8042_SECOND_PORT_IDT,I8042_SecondPort_IDT_Entry);
    }

}

void I8042_FirstPortInterruptHandler()
{
    printf("I8042: First Port Interrupt\n");
    Interrupt_SendEOI(I8042_FIRST_PORT_IDT-0x20);
}

void I8042_SecondPortInterruptHandler()
{
    printf("I8042: Second Port Interrupt\n");
    Interrupt_SendEOI(I8042_SECOND_PORT_IDT-0x20);
}

// Debugging ===================================================================

void I8042_SetDebug(uint8_t debug)
{
    I8042_Debug = debug;
}

