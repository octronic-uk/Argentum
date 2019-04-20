#include <Drivers/PS2/PS2Driver.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <Drivers/IO/IODriver.h>
#include <Drivers/Screen/ScreenDriver.h>
#include <Kernel.h>

static struct Kernel* _Kernel;
volatile bool PS2Driver_WaitingForKeyPress = false;

#define PS2_PAUSE 2500

bool PS2Driver_Constructor(struct PS2Driver* self, struct Kernel* kernel)
{
    printf("PS2: Constructing\n");

    self->Debug = false;
    self->Kernel = kernel;
    _Kernel = kernel;

    self->ResponseBufferIndex = 0;
    memset(self->ResponseBuffer, 0, sizeof(struct _Ps2Response)*PS2_RESPONSE_BUFFER_SIZE);
    self->SecondPortExists = false;
    self->KeyboardNextByteExpected = Status;

    memset(&self->FirstPort_IDT_Entry, 0, sizeof(struct Interrupt_DescriptorTableEntry));
    memset(&self->SecondPort_IDT_Entry, 0, sizeof(struct Interrupt_DescriptorTableEntry));

    PS2Driver_CMD_DisableFirstPort(self);
    PS2Driver_CMD_DisableSecondPort(self);

    PS2Driver_FlushDataBuffer(self);

    PS2Driver_SetupInterruptHandlers(self);

    PS2Driver_WriteConfigurationByte(self);
    if (PS2Driver_CMD_TestController(self))
    {
        if (PS2Driver_CMD_TestFirstPort(self))
        {
            PS2Driver_CMD_EnableFirstPort(self);
        }
        if (self->SecondPortExists)
        {
            if(PS2Driver_CMD_TestSecondPort(self))
            {
                PS2Driver_CMD_EnableSecondPort(self);
            }
        }
    }

    PS2Driver_DeviceCMD_ResetFirstPort(self);
    if (self->SecondPortExists)
    {
        PS2Driver_DeviceCMD_ResetSecondPort(self);
    }

    PS2Driver_EnableInterrupts(self);
    PS2Driver_FlushDataBuffer(self); 
    return true;
}

void PS2Driver_Destructor(struct PS2Driver* self)
{
    printf("PS2: Destructing\n");
}

// IO Function Abstractions ====================================================

void PS2Driver_FlushDataBuffer(struct PS2Driver* self)
{
    if (self->Debug)
    {
        printf("PS2: Flushing data buffer\n");
    }
    int wait = 0;
    do
    {
        PS2Driver_ReadDataPort(self);
        usleep(10);
        uint8_t status = PS2Driver_ReadStatusRegister(self);
        if ((status & PS2_STATUS_REG_OUTPUT_BUFFER_READY) != 0)
        {
            wait++;
            usleep(PS2_WAIT_FOR);
        }
        else
        {
            break;
        }
        
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error flushing data buffer\n");
            asm("hlt");
            return;
        }
    }
    while (1);
    return;
}
uint8_t PS2Driver_ReadDataPort(struct PS2Driver* self)
{
    if (self->Debug)
    {
        printf("PS2: Reading data port\n");
    }
    return IO_ReadPort8b(PS2_DATA_PORT_RW);
}

void PS2Driver_WriteDataPort(struct PS2Driver* self, uint8_t data)
{
    if (self->Debug)
    {
        printf("PS2: Writing data port 0x%x\n",data);
    }
    IO_WritePort8b(PS2_DATA_PORT_RW, data);
}

uint8_t PS2Driver_ReadStatusRegister(struct PS2Driver* self)
{
    uint8_t retval =  IO_ReadPort8b(PS2_STATUS_REGISTER_R);
    if (self->Debug)
    {
        printf("PS2: Read status register 0x%x\n",retval);
    }
    return retval;
}

void PS2Driver_WriteCommandRegister(struct PS2Driver* self, uint8_t cmd)
{
    if(self->Debug) 
    {
        printf("PS2: Writing Command Register cmd: 0x%x\n",cmd);
    }
    IO_WritePort8b(PS2_COMMAND_REGISTER_W, cmd);
}

// PS/2 Controller Commands ====================================================

void PS2Driver_CMD_EnableFirstPort(struct PS2Driver* self)
{
    if (self->Debug)
    {
        printf("PS2: Enabling First Port\n");
    }
    PS2Driver_WriteCommandRegister(self, PS2_CMD_ENABLE_FIRST_PORT);
}

void PS2Driver_CMD_DisableFirstPort(struct PS2Driver* self)
{
    if (self->Debug)
    {
        printf("PS2: Disabling First Port\n");
    }
    PS2Driver_WriteCommandRegister(self, PS2_CMD_DISABLE_FIRST_PORT);
}

void PS2Driver_CMD_EnableSecondPort(struct PS2Driver* self)
{
    if (self->Debug)
    {
        printf("PS2: Enabling Second Port\n");
    }
    PS2Driver_WriteCommandRegister(self, PS2_CMD_ENABLE_SECOND_PORT);
}

void PS2Driver_CMD_DisableSecondPort(struct PS2Driver* self)
{
    if (self->Debug)
    {
        printf("PS2: Disabling Second Port\n");
    }
    PS2Driver_WriteCommandRegister(self, PS2_CMD_DISABLE_SECOND_PORT);
}

uint8_t PS2Driver_CMD_TestController(struct PS2Driver* self)
{
    // Write command
    if (self->Debug) printf("PS2: Testing PS/2 Controller\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_TEST_CONTROLLER);
    // Wait for result to appear
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Unable to test controller\n");
            asm("hlt");
            return 0;
        }
    }
    // Read Result
    uint8_t result = PS2Driver_ReadDataPort(self);
    // Test Result
    if ((result & PS2_TEST_RESULT_PASSED) == PS2_TEST_RESULT_PASSED) 
    {
        if (self->Debug) {
            printf("PS2: Testing PS/2 Controller PASSED\n");
            usleep(PS2_PAUSE);
        }
        return 1;
    }
    else if ((result & PS2_TEST_RESULT_FAILED) == PS2_TEST_RESULT_FAILED) 
    {
        if (self->Debug) {
            printf("PS2: Testing PS/2 Controller FAILED\n");
            usleep(PS2_PAUSE);
        }
        return 0;
    }
    else 
    {
        if (self->Debug) {
            printf("PS2: Testing PS/2 Controller FLOPPED :/\n");
            usleep(PS2_PAUSE);
        }
        return 0;
    }
}

uint8_t PS2Driver_CMD_TestFirstPort(struct PS2Driver* self)
{
    // Write command
    if (self->Debug) printf("PS2: Testing First Port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_TEST_FIRST_PORT);
    // Wait for result to appear
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error - Test first port timeout\n");
            asm("hlt");
            return false;
        }
    }
    // Read Result
    uint8_t result = PS2Driver_ReadDataPort(self);
    // Test Result
     
    if (result == 0 || result == PS2_TEST_RESULT_PASSED) 
    {
        if (self->Debug) {
            printf("PS2: Testing First Port PASSED\n");
            usleep(PS2_PAUSE);
        }
        return 1;
    }
    else 
    {
        if (self->Debug) {
            printf("PS2: Testing First Port FAILED with 0x%x\n",result);
            usleep(PS2_PAUSE);
        }
        return 0;
    }
}

uint8_t PS2Driver_CMD_TestSecondPort(struct PS2Driver* self)
{
    // Write command
    if (self->Debug) printf("PS2: Testing First Port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_TEST_SECOND_PORT);
    // Wait for result to appear
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error - Timeout testing second port\n");
            asm("hlt");
            return 0;
        }
    }
    // Read Result
    uint8_t result = PS2Driver_ReadDataPort(self);
    // Test Result
    if (result == 0 || result == PS2_TEST_RESULT_PASSED) 
    {
        if (self->Debug) {
            printf("PS2: Testing Second Port PASSED\n");
            usleep(PS2_PAUSE);
        }
        return 1;
    }
    else 
    {
        if (self->Debug) {
            printf("PS2: Testing Second Port FAILED with 0x%x\n",result);
            usleep(PS2_PAUSE);
        }
        return 0;
    }
}
// Device Commands
void PS2Driver_DeviceCMD_ResetFirstPort(struct PS2Driver* self)
{
    if (self->Debug) printf("PS2: Resetting First Port\n");
    // Wait for register to clear
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) != 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error - Timeout resetting first port 1 \n");
            asm("hlt");
            break;
        }
    }
    // Read Result
    PS2Driver_WriteDataPort(self, PS2_DEVICE_RESET);
    // Wait for response
    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error - Timeout resetting first port 2\n");
            asm("hlt");
            return;
        }
    }
    uint8_t result = PS2Driver_ReadDataPort(self);
    // Test Result
    if (result == PS2_DEVICE_RESET_SUCCESS) 
    {
        if (self->Debug) 
        {
            printf("PS2: Reset first port successfully\n");
            usleep(PS2_PAUSE);
        }
    }
    else if (result == 0xAA)
    {
        if (self->Debug) 
        printf("PS2: Got 0xAA...\n");
        // Wait for response
        wait = 0;
        while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
        {
            usleep(PS2_WAIT_FOR);
            wait++;
            if (wait > PS2_TIMEOUT)
            {
                printf("PS2: Error - Timeout reading first port 2\n");
                asm("hlt");
                return;
            }
        }
        result = PS2Driver_ReadDataPort(self);
        if (result == PS2_DEVICE_RESET_SUCCESS)
        {
            if (self->Debug) 
            {
                printf("PS2: Reset finally succeeded on first port\n");
                usleep(PS2_PAUSE);
            }
        }

    }
    else if (result == PS2_DEVICE_RESET_FAILURE)
    {
        if (self->Debug) 
        {
            printf("PS2: Reset first port FAILED\n");
            usleep(PS2_PAUSE);
        }
    }
    else
    {
        if (self->Debug) 
        {
            printf("PS2: Reset first port FAILED with weird result 0x%x\n",result);
            usleep(PS2_PAUSE);
        }
    }
}
void PS2Driver_DeviceCMD_ResetSecondPort(struct PS2Driver* self)
{
    if (self->Debug) printf("PS2: Resetting Second Port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_WRITE_TO_SECOND_PORT);
    PS2Driver_WriteDataPort(self, PS2_DEVICE_RESET);
    // Wait for response
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error reading port 2, 1\n");
            asm("hlt");
            return;
        }
    }
    uint8_t result = PS2Driver_ReadDataPort(self);
    // Test Result
    if (result == PS2_DEVICE_RESET_SUCCESS) 
    {
        if (self->Debug) 
        {
            printf("PS2: Reset second port successfully\n");
            usleep(PS2_PAUSE);
        }
    }
    else if (result == 0xAA)
    {
        if (self->Debug) 
        printf("PS2: Got 0xAA...\n");
        // Wait for response
        wait = 0;
        while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
        {
            usleep(PS2_WAIT_FOR);
            wait++;
            if (wait > PS2_TIMEOUT)
            {
                printf("PS2: Error reading port 2, 2\n");
                asm("hlt");
                return;
            }
        }
        result = PS2Driver_ReadDataPort(self);
        if (result == PS2_DEVICE_RESET_SUCCESS)
        {
            if (self->Debug) 
            {
                printf("PS2: Reset finally succeeded on second port\n");
                usleep(PS2_PAUSE);
            }
        }

    }
    else if (result == PS2_DEVICE_RESET_FAILURE)
    {
        if (self->Debug) 
        {
            printf("PS2: Reset second port FAILED\n");
            usleep(PS2_PAUSE);
        }
    }
    else
    {
        if (self->Debug)
        {
            printf("PS2: Reset second port FAILED with weird result 0x%x\n",result);
            usleep(PS2_PAUSE);
        }
    }
}

void PS2Driver_WriteConfigurationByte(struct PS2Driver* self)
{
    if (self->Debug) printf("PS2: Writing config byte\n");
    
    PS2Driver_WriteCommandRegister(self, PS2_CMD_READ_RAM_BYTE_ZERO);

    uint8_t data;
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout Writing Configuration byte 1\n");
            asm("hlt");
            break;
        }
    }
    // Read Result
    uint8_t currentConfig = PS2Driver_ReadDataPort(self); 

    if (self->Debug) printf("PS2: Current config byte 0x%x\n",currentConfig);

    self->SecondPortExists = (currentConfig & PS2_CONFIG_SECOND_CLOCK) != 0;
    if (self->Debug)
    {
        if (self->SecondPortExists) 
        {
            printf("PS2: Second Port found\n");
            usleep(PS2_PAUSE);
        }
        else 
        {
            printf("PS2: Second Port NOT found\n");
            usleep(PS2_PAUSE);
        }
    }
    // disable all IRQs and disable translation (clear bits 0, 1 and 6).
    currentConfig &= 0xBC;

    PS2Driver_WriteCommandRegister(self, PS2_CMD_WRITE_RAM_BYTE_ZERO);

    wait = 0;
    // was PS2_STATUS_REG_INPUT_IS_CMD
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_INPUT_IS_CMD) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout writing configuration byte 2\n");
            asm("hlt");
            break;
        }
    }

    if (self->Debug) printf("PS2: Writing new config byte 0x%x\n",currentConfig);

    PS2Driver_WriteDataPort(self, currentConfig);
}

// Interrupt ==================================================================

bool PS2Driver_EnableInterrupts(struct PS2Driver* self)
{
    if (self->Debug) printf("PS2: Enabling Interrupts\n");
    
    PS2Driver_WriteCommandRegister(self, PS2_CMD_READ_RAM_BYTE_ZERO);

    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Unable enable interrupts\n");
            asm("hlt");
            return false;
        }
    }
    // Read Result
    uint8_t currentConfig = PS2Driver_ReadDataPort(self); 

    if (self->Debug) printf("PS2: Current config byte 0x%x\n",currentConfig);

    currentConfig |= PS2_FIRST_PORT_INTERRUPT;

    if (self->SecondPortExists)
    {
        currentConfig |= PS2_SECOND_PORT_INTERRUPT;
    }

    PS2Driver_WriteCommandRegister(self, PS2_CMD_WRITE_RAM_BYTE_ZERO);

    wait = 0;
    // was PS2_STATUS_REG_INPUT_IS_CMD
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_INPUT_IS_CMD) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Unable to enable interrupts 2\n");
            asm("hlt");
            return false;
        }
    }

    if (self->Debug) printf("PS2: Writing new config byte 0x%x\n",currentConfig);

    PS2Driver_WriteDataPort(self, currentConfig);
    return true;
}

void PS2Driver_SetupInterruptHandlers(struct PS2Driver* self)
{
    // First Port
    if (self->Debug)
    {
	   printf("PS2: Setting first port IDT entry.\n");
	}
	InterruptDriver_SetHandlerFunction(&self->Kernel->Interrupt, 1,PS2Driver_FirstPortInterruptHandler);

    // First Port
    if (self->SecondPortExists)
    {
        if (self->Debug)
        {
            printf("PS2: Setting second port IDT entry.\n");
        }
        InterruptDriver_SetHandlerFunction(&self->Kernel->Interrupt, 12,PS2Driver_SecondPortInterruptHandler);
    }
}

void PS2Driver_FirstPortInterruptHandler()
{
    static uint8_t last = 0x00;
    struct PS2Driver* self = &_Kernel->PS2;
    if (self->Debug) 
    {
        printf("PS2: First Port Interrupt\n");
    }
    if ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) != 0) 
    {
        uint8_t keycode = PS2Driver_ReadDataPort(self);
        if (self->Debug) 
        {
            printf("PS2: Got Keyboard Event scancode: 0x%x\n",keycode);
        }
        if (last == 0xF0 && keycode == 0x29)
        {
            PS2Driver_WaitingForKeyPress = 0;
        }
        last = keycode;
    }
}

void PS2Driver_SecondPortInterruptHandler()
{
    struct PS2Driver* self = &_Kernel->PS2;
    if (self->Debug)  
    {
        printf("PS2: Second Port Interrupt\n");
    }
    uint8_t data = PS2Driver_ReadDataPort(self);
    if (self->Debug) 
    {
        printf("PS2: Got data on second port 0x%x\n",data);
    }
}

// Debugging ===================================================================

void PS2Driver_SetDebug(struct PS2Driver* self, uint8_t debug)
{
    self->Debug = debug;
}

void PS2Driver_WaitForKeyPress(const char* msg)
{
    printf("%s Press 'Space' to continue... (%d)\n", msg, _Kernel->PIT.Ticks );
    PS2Driver_WaitingForKeyPress = 1;
    while  (PS2Driver_WaitingForKeyPress) {}
}