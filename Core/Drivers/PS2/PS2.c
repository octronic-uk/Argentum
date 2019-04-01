#include "PS2.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <Drivers/IO/IO.h>
#include <Drivers/Screen/Screen.h>
#include <Kernel.h>

extern struct Kernel kernel;

void PS2_Constructor(struct PS2* self, struct Interrupt* interrupt)
{
    printf("PS2: Constructing\n");
    self->Interrupt = interrupt;
    self->Debug = false;
    self->SecondPortExists = false;
    self->KeyboardNextByteExpected = Status;
    self->WaitingForKeyPress = false;

    memset(&self->FirstPort_IDT_Entry, 0, sizeof(struct Interrupt_DescriptorTableEntry));
    memset(&self->SecondPort_IDT_Entry, 0, sizeof(struct Interrupt_DescriptorTableEntry));

    PS2_SetupInterruptHandlers(self);
    PS2_CMD_DisableFirstPort(self);
    PS2_CMD_DisableSecondPort(self);
    PS2_ReadDataPort(self); // to flush the output buffer
    PS2_WriteConfigurationByte(self);
    if (PS2_CMD_TestController(self))
    {
        if (PS2_CMD_TestFirstPort(self))
        {
            PS2_CMD_EnableFirstPort(self);
        }
        if (self->SecondPortExists)
        {
            if(PS2_CMD_TestSecondPort(self))
            {
                PS2_CMD_EnableSecondPort(self);
            }
        }
    }
    PS2_SetupInterruptHandlers(self);

    PS2_DeviceCMD_ResetFirstPort(self);
    if (self->SecondPortExists)
    {
        PS2_DeviceCMD_ResetSecondPort(self);
    }

    PS2_EnableInterrupts(self);

    // Empty the buffer hard for good measure
    PS2_ReadDataPort(self); // to flush the output buffer
    PS2_ReadDataPort(self); // to flush the output buffer
    PS2_ReadDataPort(self); // to flush the output buffer
    PS2_ReadDataPort(self); // to flush the output buffer
}

void PS2_Destructor(struct PS2* self)
{
    printf("PS2: Destructing\n");
}

// IO Function Abstractions ====================================================
uint8_t PS2_ReadDataPort(struct PS2* self)
{
    return IO_ReadPort8b(PS2_DATA_PORT_RW);
}

void PS2_WriteDataPort(struct PS2* self, uint8_t data)
{
    IO_WritePort8b(PS2_DATA_PORT_RW, data);
}

uint8_t PS2_ReadStatusRegister(struct PS2* self)
{
    return IO_ReadPort8b(PS2_STATUS_REGISTER_R);
}

void PS2_WriteCommandRegister(struct PS2* self, uint8_t cmd)
{
    IO_WritePort8b(PS2_COMMAND_REGISTER_W, cmd);
}

// PS/2 Controller Commands ====================================================

void PS2_CMD_EnableFirstPort(struct PS2* self)
{
    if (self->Debug)
    {
        printf("PS2: Enabling First Port\n");
    }
    PS2_WriteCommandRegister(self, PS2_CMD_ENABLE_FIRST_PS2_PORT);
}

void PS2_CMD_DisableFirstPort(struct PS2* self)
{
    if (self->Debug)
    {
        printf("PS2: Disabling First Port\n");
    }
    PS2_WriteCommandRegister(self, PS2_CMD_DISABLE_FIRST_PS2_PORT);
}

void PS2_CMD_EnableSecondPort(struct PS2* self)
{
    if (self->Debug)
    {
        printf("PS2: Enabling Second Port\n");
    }
    PS2_WriteCommandRegister(self, PS2_CMD_ENABLE_SECOND_PS2_PORT);
}

void PS2_CMD_DisableSecondPort(struct PS2* self)
{
    if (self->Debug)
    {
        printf("PS2: Disabling Second Port\n");
    }
    PS2_WriteCommandRegister(self, PS2_CMD_DISABLE_SECOND_PS2_PORT);
}

uint8_t PS2_CMD_TestController(struct PS2* self)
{
    // Write command
    if (self->Debug) printf("PS2: Testing PS/2 Controller\n");
    PS2_WriteCommandRegister(self, PS2_CMD_TEST_PS2_CONTROLLER);
    // Wait for result to appear
    while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER) == 0)
    {
        usleep(20);
    }
    // Read Result
    uint8_t result = PS2_ReadDataPort(self);
    // Test Result
    if ((result & PS2_TEST_RESULT_PASSED) == PS2_TEST_RESULT_PASSED) 
    {
        if (self->Debug) printf("PS2: Testing PS/2 Controller PASSED\n");
        return 1;
    }
    else if ((result & PS2_TEST_RESULT_FAILED) == PS2_TEST_RESULT_FAILED) 
    {
        if (self->Debug) printf("PS2: Testing PS/2 Controller FAILED\n");
        return 0;
    }
    else 
    {
        if (self->Debug) printf("PS2: Testing PS/2 Controller FLOPPED :/\n");
        return 0;
    }
}

uint8_t PS2_CMD_TestFirstPort(struct PS2* self)
{
    // Write command
    if (self->Debug) printf("PS2: Testing First Port\n");
    PS2_WriteCommandRegister(self, PS2_CMD_TEST_FIRST_PS2_PORT);
    // Wait for result to appear
    while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER) == 0)
    {
        usleep(20);
    }
    // Read Result
    uint8_t result = PS2_ReadDataPort(self);
    // Test Result
     
    if (result == 0 || result == PS2_TEST_RESULT_PASSED) 
    {
        if (self->Debug) printf("PS2: Testing First Port PASSED\n");
        return 1;
    }
    else 
    {
        if (self->Debug) printf("PS2: Testing First Port FAILED with 0x%x\n",result);
        return 0;
    }
}

uint8_t PS2_CMD_TestSecondPort(struct PS2* self)
{
    // Write command
    if (self->Debug) printf("PS2: Testing First Port\n");
    PS2_WriteCommandRegister(self, PS2_CMD_TEST_SECOND_PS2_PORT);
    // Wait for result to appear
    while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER) == 0)
    {
        usleep(20);
    }
    // Read Result
    uint8_t result = PS2_ReadDataPort(self);
    // Test Result
    if (result == 0 || result == PS2_TEST_RESULT_PASSED) 
    {
        if (self->Debug) printf("PS2: Testing Second Port PASSED\n");
        return 1;
    }
    else 
    {
        if (self->Debug) printf("PS2: Testing Second Port FAILED with 0x%x\n",result);
        return 0;
    }
}
// Device Commands
void PS2_DeviceCMD_ResetFirstPort(struct PS2* self)
{
    if (self->Debug) printf("PS2: Resetting First Port\n");
    // Wait for register to clear
    while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER) != 0)
    {
        usleep(20);
    }
    // Read Result
    PS2_WriteDataPort(self, PS2_DEVICE_RESET);
    // Wait for response
    while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER) == 0)
    {
        usleep(20);
    }
    uint8_t result = PS2_ReadDataPort(self);
    // Test Result
    if (result == PS2_DEVICE_RESET_SUCCESS) 
    {
        if (self->Debug) 
        printf("PS2: Reset first port successfully\n");
    }
    else if (result == 0xAA)
    {
        if (self->Debug) 
        printf("PS2: Got 0xAA...\n");
        // Wait for response
        while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER) == 0)
        {
            usleep(20);
        }
        result = PS2_ReadDataPort(self);
        if (result == PS2_DEVICE_RESET_SUCCESS)
        {
            if (self->Debug) 
            printf("PS2: Reset finally succeeded on first port\n");
        }

    }
    else if (result == PS2_DEVICE_RESET_FAILURE)
    {
        if (self->Debug) 
        printf("PS2: Reset first port FAILED\n");
    }
    else
    {
        if (self->Debug) 
        printf("PS2: Reset first port FAILED with weird result 0x%x\n",result);
    }
}
void PS2_DeviceCMD_ResetSecondPort(struct PS2* self)
{
    if (self->Debug) printf("PS2: Resetting Second Port\n");
    PS2_WriteCommandRegister(self, PS2_CMD_WRITE_TO_SECOND_PS2_PORT);
    PS2_WriteDataPort(self, PS2_DEVICE_RESET);
    // Wait for response
    while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER) == 0)
    {
        usleep(20);
    }
    uint8_t result = PS2_ReadDataPort(self);
    // Test Result
    if (result == PS2_DEVICE_RESET_SUCCESS) 
    {
        if (self->Debug) 
        printf("PS2: Reset second port successfully\n");
    }
    else if (result == 0xAA)
    {
        if (self->Debug) 
        printf("PS2: Got 0xAA...\n");
        // Wait for response
        while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER) == 0)
        {
            usleep(20);
        }
        result = PS2_ReadDataPort(self);
        if (result == PS2_DEVICE_RESET_SUCCESS)
        {
            if (self->Debug) 
            printf("PS2: Reset finally succeeded on second port\n");
        }

    }
    else if (result == PS2_DEVICE_RESET_FAILURE)
    {
        if (self->Debug) 
        printf("PS2: Reset second port FAILED\n");
    }
    else
    {
        if (self->Debug)
        printf("PS2: Reset second port FAILED with weird result 0x%x\n",result);
    }
}

void PS2_WriteConfigurationByte(struct PS2* self)
{
    if (self->Debug) printf("PS2: Writing config byte\n");
    
    PS2_WriteCommandRegister(self, PS2_CMD_READ_RAM_BYTE_ZERO);

    while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_CMD_OR_DATA) == 0)
    {
        usleep(20);
    }
    // Read Result
    uint8_t currentConfig = PS2_ReadDataPort(self); 

    if (self->Debug) printf("PS2: Current config byte 0x%x\n",currentConfig);

    self->SecondPortExists = (currentConfig & PS2_CONFIG_SECOND_PS2_CLOCK) != 0;

    currentConfig = currentConfig & 0xDC;

    PS2_WriteCommandRegister(self, PS2_CMD_WRITE_RAM_BYTE_ZERO);

    while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_CMD_OR_DATA) == 0 )
    {
        usleep(20);
    }

    if (self->Debug) printf("PS2: Writing new config byte 0x%x\n",currentConfig);

    PS2_WriteDataPort(self, currentConfig);

    if (self->Debug)
    {
        if (self->SecondPortExists) printf("PS2: Second Port found\n");
        else printf("PS2: Second Port NOT found\n");
    }
}

// Interrupt ==================================================================

void PS2_EnableInterrupts(struct PS2* self)
{
    if (self->Debug) printf("PS2: Enabling Interrupts\n");
    
    PS2_WriteCommandRegister(self, PS2_CMD_READ_RAM_BYTE_ZERO);

    while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_CMD_OR_DATA) == 0)
    {
        usleep(20);
    }
    // Read Result
    uint8_t currentConfig = PS2_ReadDataPort(self); 

    if (self->Debug) printf("PS2: Current config byte 0x%x\n",currentConfig);

    currentConfig |= PS2_FIRST_PORT_INTERRUPT;

    if (self->SecondPortExists)
    {
        currentConfig |= PS2_SECOND_PORT_INTERRUPT;
    }

    PS2_WriteCommandRegister(self, PS2_CMD_WRITE_RAM_BYTE_ZERO);

    while ((PS2_ReadStatusRegister(self) & PS2_STATUS_REG_CMD_OR_DATA) == 0 )
    {
        usleep(20);
    }

    if (self->Debug) printf("PS2: Writing new config byte 0x%x\n",currentConfig);

    PS2_WriteDataPort(self, currentConfig);
}

void PS2_SetupInterruptHandlers(struct PS2* self)
{
    // First Port
    if (self->Debug)
    {
	   printf("PS2: Setting first port IDT entry.\n");
	}
	Interrupt_SetHandlerFunction(self->Interrupt, 1,PS2_FirstPortInterruptHandler);

    // First Port
    if (self->SecondPortExists)
    {
        if (self->Debug)
        {
            printf("PS2: Setting second port IDT entry.\n");
        }
        Interrupt_SetHandlerFunction(self->Interrupt, 12,PS2_SecondPortInterruptHandler);
    }
}

void PS2_FirstPortInterruptHandler()
{
    struct PS2* self = &kernel.PS2;
    if (self->Debug) printf("PS2: First Port Interrupt\n");
    if (PS2_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER) 
    {
        int8_t keycode = (int8_t)PS2_ReadDataPort(self);
        if (keycode >= 0)
        {
            if (self->Debug) printf("Got Keyboard Event scancode: 0x%x\n",keycode);

            self->WaitingForKeyPress = 0;
        }
    }
}

void PS2_SecondPortInterruptHandler()
{
    struct PS2* self = &kernel.PS2;
    if (self->Debug)  
    {
        printf("PS2: Second Port Interrupt\n");
    }
    uint8_t data = PS2_ReadDataPort(self);
    if (self->Debug) 
    {
        printf("PS2: Got data on second port 0x%x\n",data);
    }
}

// Debugging ===================================================================

void PS2_SetDebug(struct PS2* self, uint8_t debug)
{
    self->Debug = debug;
}


void PS2_WaitForKeyPress(struct PS2* self)
{
    printf("\nPS2: Waiting for Key Press...\n\n");
    self->WaitingForKeyPress = 1;
    while  (self->WaitingForKeyPress) {}
}