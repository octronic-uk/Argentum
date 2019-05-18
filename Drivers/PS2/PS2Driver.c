#include <Drivers/PS2/PS2Driver.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <Drivers/IO/IODriver.h>
#include <Drivers/Screen/TextMode/TextModeDriver.h>
#include <Objects/Kernel/Kernel.h>

extern Kernel _Kernel;
#define io_pause usleep(5);

bool PS2Driver_Constructor(PS2Driver* self)
{
    printf("PS2 Driver: Constructing\n");
    self->Debug = false;
    self->SecondPortExists = false;
    ScancodeParser_Constructor(&self->ScancodeParser);
    MouseState_Constructor(&self->MouseState);

    PS2Driver_CMD_DisableFirstPort(self);
    PS2Driver_CMD_DisableSecondPort(self);
    PS2Driver_FlushDataBuffer(self);
    PS2Driver_WriteInitialConfigurationByte(self);
    PS2Driver_SetupInterruptHandlers(self);

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

/*    PS2Driver_IdentifyPort1(self);

    if (self->SecondPortExists)
    {
        PS2Driver_IdentifyPort2(self);
    }
    */

    PS2Driver_EnableInterrupts(self);

    PS2Driver_DeviceCMD_ResetFirstPort(self);
    if (self->SecondPortExists)
    {
        PS2Driver_DeviceCMD_ResetSecondPort(self);
    }


    PS2Driver_FlushDataBuffer(self);

    PS2Driver_MouseEnable(self);
    
    return true;
}

void PS2Driver_Destructor(PS2Driver* self)
{
    printf("PS2: Destructing\n");
}

// IO Function Abstractions ====================================================

uint8_t PS2Driver_ReadResponseFromSecondPort(PS2Driver* self)
{
    printf("PS2: > Read response from second port\n");
    return 0;
}

bool PS2Driver_WriteCommandToSecondPort(PS2Driver* self, uint8_t cmd)
{
    if(self->Debug) printf("PS2: > Write command to second port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_WRITE_TO_SECOND_IN_BUFFER);
    io_pause

    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) != 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ok to write port 2\n");
            return false;
        }
    }

    // Send command
    PS2Driver_WriteDataPort(self, cmd);
    io_pause

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ack on port 2\n");
            return false;
        }
    }
    // Read response
    uint8_t response = PS2Driver_ReadDataPort(self);
    return (response == PS2_CMD_RESPONSE_ACK);
}

void PS2Driver_FlushDataBuffer(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Flushing data buffer\n");
    int wait = 0;
    do
    {
        PS2Driver_ReadDataPort(self);
        io_pause
        uint8_t status = PS2Driver_ReadStatusRegister(self);
        io_pause
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
            abort();
            return;
        }
    }
    while (1);
    if(self->Debug) printf("PS2: = OK flushing data buffer\n");
    return;
}

uint8_t PS2Driver_ReadDataPort(PS2Driver* self)
{
    if (self->Debug) printf("PS2: Reading data port\n");
    return IO_ReadPort8b(PS2_DATA_PORT_RW);
}

void PS2Driver_WriteDataPort(PS2Driver* self, uint8_t data)
{
    if (self->Debug) printf("PS2: Writing data port 0x%x\n",data);
    IO_WritePort8b(PS2_DATA_PORT_RW, data);
}

uint8_t PS2Driver_ReadStatusRegister(PS2Driver* self)
{
    uint8_t retval =  IO_ReadPort8b(PS2_STATUS_REGISTER_R);
    io_pause
    if (self->Debug) printf("PS2: Read status register 0x%x\n",retval);
    return retval;
}

void PS2Driver_WriteCommandRegister(PS2Driver* self, uint8_t cmd)
{
    if(self->Debug) printf("PS2: Writing Command Register cmd: 0x%x\n",cmd);
    IO_WritePort8b(PS2_COMMAND_REGISTER_W, cmd);
}

// PS/2 Controller Commands ====================================================

void PS2Driver_CMD_EnableFirstPort(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Enabling First Port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_ENABLE_FIRST_PORT);
    io_pause
    if (self->Debug) printf("PS2: = OK Enabling First Port\n");
}

void PS2Driver_CMD_DisableFirstPort(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Disabling First Port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_DISABLE_FIRST_PORT);
    io_pause
    if (self->Debug) printf("PS2: = OK Disabling First Port\n");
}

void PS2Driver_CMD_EnableSecondPort(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Enabling Second Port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_ENABLE_SECOND_PORT);
    io_pause
    if (self->Debug) printf("PS2: = OK Enabling Second Port\n");
}

void PS2Driver_CMD_DisableSecondPort(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Disabling Second Port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_DISABLE_SECOND_PORT);
    io_pause
    if (self->Debug) printf("PS2: = OK Disabling Second Port\n");
}

uint8_t PS2Driver_CMD_TestController(PS2Driver* self)
{
    // Write command
    if (self->Debug) printf("PS2: > Testing PS/2 Controller\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_TEST_CONTROLLER);
    io_pause
    // Wait for result to appear
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Unable to test controller\n");
            abort();
            return 0;
        }
    }
    // Read Result
    uint8_t result = PS2Driver_ReadDataPort(self);
    io_pause
    // Test Result
    if ((result & PS2_TEST_RESULT_PASSED) == PS2_TEST_RESULT_PASSED) 
    {
        if (self->Debug) printf("PS2: = Testing PS/2 Controller PASSED\n");
        return 1;
    }
    else if ((result & PS2_TEST_RESULT_FAILED) == PS2_TEST_RESULT_FAILED) 
    {
        if (self->Debug) printf("PS2: = Testing PS/2 Controller FAILED\n");
        return 0;
    }
    else 
    {
        if (self->Debug) printf("PS2: = Testing PS/2 Controller FLOPPED :/\n");
        return 0;
    }
}

uint8_t PS2Driver_CMD_TestFirstPort(PS2Driver* self)
{
    // Write command
    if (self->Debug) printf("PS2: > Testing First Port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_TEST_FIRST_PORT);
    io_pause
    // Wait for result to appear
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error - Test first port timeout\n");
            abort();
            return false;
        }
    }
    // Read Result
    uint8_t result = PS2Driver_ReadDataPort(self);
    io_pause
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

uint8_t PS2Driver_CMD_TestSecondPort(PS2Driver* self)
{
    // Write command
    if (self->Debug) printf("PS2: > Testing Second Port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_TEST_SECOND_PORT);
    io_pause
    // Wait for result to appear
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error - Timeout testing second port\n");
            abort();
            return 0;
        }
    }
    // Read Result
    uint8_t result = PS2Driver_ReadDataPort(self);
    io_pause
    // Test Result
    if (result == 0) 
    {
        if (self->Debug) printf("PS2: Testing Second Port PASSED with 0x%x\n",result);
        return 1;
    }
    else 
    {
        if (self->Debug) printf("PS2: Testing Second Port FAILED with 0x%x\n",result);
        return 0;
    }
}
// Device Commands
void PS2Driver_DeviceCMD_ResetFirstPort(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Resetting First Port\n");
    // Wait for register to clear
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) != 0)
    {
        PS2Driver_ReadDataPort(self);
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error - Timeout resetting first port 1 \n");
            abort();
            break;
        }
    }
    // Read Result
    PS2Driver_WriteDataPort(self, PS2_DEVICE_RESET);
    io_pause
    // Wait for response
    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error - Timeout resetting first port 2\n");
            abort();
            return;
        }
    }
    uint8_t result = PS2Driver_ReadDataPort(self);
    io_pause
    // Test Result
    if (result == PS2_DEVICE_RESET_SUCCESS) 
    {
        if (self->Debug) printf("PS2: Reset first port successfully\n");
    }
    else if (result == 0xAA)
    {
        if (self->Debug) printf("PS2: Got 0xAA...\n");
        // Wait for response
        wait = 0;
        while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
        {
            usleep(PS2_WAIT_FOR);
            wait++;
            if (wait > PS2_TIMEOUT)
            {
                printf("PS2: Error - Timeout reading first port 2\n");
                abort();
                return;
            }
        }
        result = PS2Driver_ReadDataPort(self);
        io_pause
        if (result == PS2_DEVICE_RESET_SUCCESS)
        {
            if (self->Debug) printf("PS2: Reset finally succeeded on first port\n");
        }

    }
    else if (result == PS2_DEVICE_RESET_FAILURE)
    {
        if (self->Debug) printf("PS2: Reset first port FAILED\n");
    }
    else
    {
        if (self->Debug) printf("PS2: Reset first port FAILED with weird result 0x%x\n",result);
    }
}
void PS2Driver_DeviceCMD_ResetSecondPort(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Resetting Second Port\n");
    PS2Driver_WriteCommandRegister(self, PS2_CMD_WRITE_TO_SECOND_IN_BUFFER);
    io_pause
    // Wait for register to clear
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) != 0)
    {
        PS2Driver_ReadDataPort(self);
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error - Timeout resetting second port 1 \n");
            abort();
            break;
        }
    }
    // Read Result
    PS2Driver_WriteDataPort(self, PS2_DEVICE_RESET);
    io_pause
    // Wait for response
    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT)
        {
            printf("PS2: Error - Timeout resetting second port 2\n");
            abort();
            return;
        }
    }
    uint8_t result = PS2Driver_ReadDataPort(self);
    io_pause
    // Test Result
    if (result == PS2_DEVICE_RESET_SUCCESS) 
    {
        // Wait for response
        wait = 0;
        while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
        {
            usleep(PS2_WAIT_FOR);
            wait++;
            if (wait > PS2_TIMEOUT)
            {
                printf("PS2: Error - Timeout reading second port 3\n");
                abort();
                return;
            }
        }
        result = PS2Driver_ReadDataPort(self);
        io_pause
        if (result == 0xAA)
        {
            if (self->Debug) printf("PS2: Reset in progress 0xAA\n");
            // Wait for response
            wait = 0;
            while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
            {
                usleep(PS2_WAIT_FOR);
                wait++;
                if (wait > PS2_TIMEOUT)
                {
                    printf("PS2: Error - Timeout reading second port 3\n");
                    abort();
                    return;
                }
            }
            result = PS2Driver_ReadDataPort(self);
            io_pause
            if (result == 0x00)
            {
                if (self->Debug) printf("PS2: Looks good 0x00\n");
                return;
            }
        } 

        if (self->Debug) printf("PS2: Reset second port successfully\n");
    }
    else if (result == 0xAA)
    {
        if (self->Debug) printf("PS2: Got 0xAA...\n");
        // Wait for response
        wait = 0;
        while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
        {
            usleep(PS2_WAIT_FOR);
            wait++;
            if (wait > PS2_TIMEOUT)
            {
                printf("PS2: Error - Timeout reading second port 3\n");
                abort();
                return;
            }
        }
        result = PS2Driver_ReadDataPort(self);
        io_pause
        if (result == PS2_DEVICE_RESET_SUCCESS)
        {
            if (self->Debug) printf("PS2: Reset finally succeeded on second port\n");
        }
    }
    else if (result == PS2_DEVICE_RESET_FAILURE)
    {
        if (self->Debug) printf("PS2: Reset second port FAILED\n");
    }
    else
    {
        if (self->Debug) printf("PS2: Reset second port FAILED with weird result 0x%x\n",result);
    }
}

void PS2Driver_WriteInitialConfigurationByte(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Writing INITIAL config byte\n");
    
    PS2Driver_WriteCommandRegister(self, PS2_CMD_READ_RAM_BYTE_ZERO);
    io_pause

    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout Writing Configuration byte 1\n");
            abort();
            break;
        }
    }
    // Read Result
    uint8_t currentConfig = PS2Driver_ReadDataPort(self); 
    io_pause

    if (self->Debug) printf("PS2: Current config byte 0x%x\n",currentConfig);

    self->SecondPortExists = (currentConfig & PS2_CONFIG_SECOND_CLOCK) != 0;
    if (self->Debug)
    {
        if (self->SecondPortExists) printf("PS2: Second Port found\n");
        else printf("PS2: Second Port NOT found\n");
    }
    // disable all IRQs and disable translation (clear bits 0, 1 and 6).
    currentConfig &= ~(1 | 1<<1 | 1<<5);

    PS2Driver_WriteCommandRegister(self, PS2_CMD_WRITE_RAM_BYTE_ZERO);
    io_pause

    wait = 0;
    // was PS2_STATUS_REG_INPUT_IS_CMD
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_INPUT_IS_CMD) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout writing configuration byte 2\n");
            abort();
            break;
        }
    }

    if (self->Debug) printf("PS2: Writing new config byte 0x%x\n",currentConfig);

    PS2Driver_WriteDataPort(self, currentConfig);
    io_pause

    // Check

    PS2Driver_WriteCommandRegister(self, PS2_CMD_READ_RAM_BYTE_ZERO);
    io_pause

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout Writing Configuration byte 1\n");
            abort();
            break;
        }
    }
    // Read Result
    uint8_t writtenConfig = PS2Driver_ReadDataPort(self); 

    if (writtenConfig != currentConfig)
    {
        printf("PS2: Error writing initial config, result did not match\n");
        abort();
    }

    if (self->Debug) printf("PS2: = OK Writing config byte\n",currentConfig);
}

void PS2Driver_IdentifyPort1(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Identify Port 1\n");
    
    PS2Driver_WriteDataPort(self, PS2_CMD_DISABLE_SCANNING);
    io_pause

    uint8_t data;
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for disable scanning ack on port 1\n");
            abort();
            break;
        }
    }

    data = PS2Driver_ReadDataPort(self);
    io_pause

    if (data != PS2_CMD_RESPONSE_ACK)
    {
        printf("PS2Driver Error - Port 1 Didn't get the ole ACK got 0x%x\n",data);
        return;
    }


    PS2Driver_WriteDataPort(self, PS2_CMD_IDENTIFY);
    io_pause

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ack after identifyon port 1\n");
            abort();
            break;
        }
    }

    data = PS2Driver_ReadDataPort(self);
    io_pause

    if (data != PS2_CMD_RESPONSE_ACK)
    {
        printf("PS2Driver Error - Port 1 Didn't get the ole ACK got 0x%x\n",data);
        return;
    }
    else
    {
        if (self->Debug) printf("PS2: waiting for port 1 ID data...\n");
    }

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ack after identifyon port 1\n");
            return;
        }
    }

    uint8_t id_data[3];

    id_data[0] = PS2Driver_ReadDataPort(self);
    io_pause
    if (self->Debug) printf("PS2: Port 1 got ack byte 1 0x%x\n",data);

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ack after identifyon port 1\n");
            return;
        }
    }

    id_data[1] = PS2Driver_ReadDataPort(self);
    io_pause

    if (self->Debug) printf("PS2: Port 1 got ack byte 2 0x%x\n",data);

    if (id_data[0] == 0xAB && id_data[1] == 0x83)
    {
        printf("PS2: = First Device identified as MF2 Keyboard\n");
    }

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ack after identifyon port 1\n");
            return;
        }
    }
    PS2Driver_FlushDataBuffer(self);
}

void PS2Driver_IdentifyPort2(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Identify Port 2\n");

    // Address port 2
    
    PS2Driver_WriteCommandRegister(self, PS2_CMD_WRITE_TO_SECOND_IN_BUFFER);
    io_pause

    uint8_t data;
    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) != 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ok to write port 2\n");
            break;
        }
    }

    // Send command

    PS2Driver_WriteDataPort(self, PS2_CMD_DISABLE_SCANNING);
    io_pause

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for disable scanning ack on port 2\n");
            abort();
            break;
        }
    }

    // Read response

    data = PS2Driver_ReadDataPort(self);
    io_pause

    if (data != PS2_CMD_RESPONSE_ACK)
    {
        printf("PS2Driver Error - Port2 Didn't get the ole ACK got 0x%x\n",data);
        return;
    }

    // Address port 2

    PS2Driver_WriteCommandRegister(self, PS2_CMD_WRITE_TO_SECOND_IN_BUFFER);
    io_pause

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) != 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ok to write port 2\n");
            break;
        }
    }

    // Write Command

    PS2Driver_WriteDataPort(self, PS2_CMD_IDENTIFY);
    io_pause

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ack after identifyon port 2\n");
            abort();
            break;
        }
    }

    // Read response

    data = PS2Driver_ReadDataPort(self);
    io_pause

    if (data != PS2_CMD_RESPONSE_ACK)
    {
        printf("PS2Driver Error - Port2 Didn't get the ole ACK got 0x%x\n",data);
        return;
    }
    else
    {
        if (self->Debug) printf("PS2: waiting for port 2 ID data...\n");
    }

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ack after identifyon port 2\n");
            return;
        }
    }

    // Get ID
    uint8_t id_data[2];

    id_data[0] = PS2Driver_ReadDataPort(self);
    io_pause
    if (self->Debug) printf("PS2: Port2 got ack byte 1 0x%x\n",id_data[0]);

    if (id_data[0] == 0x00)
    {
        printf("PS2: = Second Device identified as Standard PS/2 Mouse\n");
        PS2Driver_FlushDataBuffer(self);
        return;
    }

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Timeout waiting for ack after identifyon port 2\n");
            return;
        }
    }

    id_data[1] = PS2Driver_ReadDataPort(self);
    io_pause

    if (self->Debug) printf("PS2: Port2 got ack byte 2 0x%x\n",id_data[1]);
}


bool PS2Driver_MouseEnable(PS2Driver* self)
{
    if(self->Debug) printf("PS2: > Mouse Enable\n");
    if (!PS2Driver_WriteCommandToSecondPort(self, PS2_MOUSE_CMD_ENABLE_DATA_REPORTING))
    {
        printf("PS2: Error - unable to enable mouse data reporting\n");
        return false;
    }
}

// Interrupt ==================================================================

bool PS2Driver_EnableInterrupts(PS2Driver* self)
{
    if (self->Debug) printf("PS2: > Enabling Interrupts\n");
    
    PS2Driver_WriteCommandRegister(self, PS2_CMD_READ_RAM_BYTE_ZERO);
    io_pause

    int wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Unable enable interrupts\n");
            abort();
            return false;
        }
    }
    io_pause
    // Read Result
    uint8_t currentConfig = PS2Driver_ReadDataPort(self); 
    io_pause

    if (self->Debug) printf("PS2: Current config byte 0x%x\n",currentConfig);

    currentConfig |= PS2_FIRST_PORT_INTERRUPT;

    if (self->SecondPortExists)
    {
        currentConfig |= PS2_SECOND_PORT_INTERRUPT;
    }

    PS2Driver_WriteCommandRegister(self, PS2_CMD_WRITE_RAM_BYTE_ZERO);
    io_pause

    wait = 0;
    // was PS2_STATUS_REG_INPUT_IS_CMD
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_INPUT_IS_CMD) == 0 )
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Unable to enable interrupts 2\n");
            abort();
            return false;
        }
    }

    if (self->Debug) printf("PS2: Writing new config byte 0x%x\n",currentConfig);

    PS2Driver_WriteDataPort(self, currentConfig);
    io_pause

    // Check new conf
    PS2Driver_WriteCommandRegister(self, PS2_CMD_READ_RAM_BYTE_ZERO);
    io_pause

    wait = 0;
    while ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) == 0)
    {
        usleep(PS2_WAIT_FOR);
        wait++;
        if (wait > PS2_TIMEOUT) 
        {
            printf("PS2: Error - Unable enable interrupts\n");
            abort();
            return false;
        }
    }

    // Read Result
    uint8_t newConfig = PS2Driver_ReadDataPort(self); 
    io_pause
    if(self->Debug) printf("PS2: = Config is now 0x%x\n",newConfig);
    return true;
}

void PS2Driver_SetupInterruptHandlers(PS2Driver* self)
{
    // First Port
    if (self->Debug) printf("PS2: > Setting first port Interrupt Handler Function.\n");
	InterruptDriver_SetHandlerFunction(&_Kernel.Interrupt, 1,PS2Driver_FirstPortInterruptHandler);

    // Second Port
    if (self->Debug) printf("PS2: > Setting second port Interrupt Handler Function.\n");
    InterruptDriver_SetHandlerFunction(&_Kernel.Interrupt, 12,PS2Driver_SecondPortInterruptHandler);
}

void PS2Driver_FirstPortInterruptHandler()
{
    PS2Driver* self = &_Kernel.PS2;
    if (self->Debug) printf("PS2: > First Port Interrupt\n");
    if ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) != 0) 
    {
        uint8_t keycode = PS2Driver_ReadDataPort(self);
        if (self->Debug) printf("PS2: Got Keyboard Event scancode: 0x%x\n",keycode);

        ScancodeParser_ParseScancode(&self->ScancodeParser,keycode);
    }
    else
    {
        if(self->Debug) printf("PS2: First port interrupt, no data\n");
    }
}

void PS2Driver_SecondPortInterruptHandler()
{
    PS2Driver* self = &_Kernel.PS2;
    if (self->Debug) printf("PS2: > Second Port Interrupt\n");
    if ((PS2Driver_ReadStatusRegister(self) & PS2_STATUS_REG_OUTPUT_BUFFER_READY) != 0) 
    {
        uint8_t data = PS2Driver_ReadDataPort(self);
        if (self->Debug) printf("PS2: Got Mouse Data: 0x%x\n",data);
        MouseState_OnData(&self->MouseState, data);
    }
    else
    {
        if(self->Debug) printf("PS2: Second port interrupt, no data\n");
    }
}


void PS2Driver_SetKeyboardEventCallback(PS2Driver* self, void(*callback)(KeyboardEvent))
{
    ScancodeParser_SetEventCallback(&self->ScancodeParser, callback);
}

void PS2Driver_SetMouseEventCallback(PS2Driver* self, void(*callback)(MouseEvent))
{
    MouseState_SetEventCallback(&self->MouseState, callback);
}