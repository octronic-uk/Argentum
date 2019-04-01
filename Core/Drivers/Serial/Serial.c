#include "Serial.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <Kernel.h>
#include <Drivers/IO/IO.h>
#include <Drivers/Interrupt/Interrupt.h>


extern struct Kernel kernel;

void Serial_Constructor(struct Serial* self, struct Interrupt* interrupt)
{
    printf("Serial: Constructing\n");
    self->Debug = 0;
    self->Interrupt = interrupt;
    Serial_SetPortAddressTable(self);
    Serial_SetupPort(self, &Serial_Port1_8N1);
	Serial_SetInterruptEnableRegister(self, &Serial_Port1_8N1);
}

void Serial_Destructor(struct Serial* self)
{
    printf("Serial: Destructing\n");
}

// Helper Functions ============================================================

void Serial_SetPortAddressTable(struct Serial* self)
{
    if (self->Debug) printf("Serial: Setting Port Address Table\n");
    self->PortAddresses[0] = SERIAL_PORT_1_ADDRESS;
    self->PortAddresses[1] = SERIAL_PORT_2_ADDRESS;
    self->PortAddresses[2] = SERIAL_PORT_3_ADDRESS;
    self->PortAddresses[3] = SERIAL_PORT_4_ADDRESS;
}

uint16_t Serial_GetDivisorFromBaudRate(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    Serial_BaudRate baudRate = desc->mBaudRate;
    switch(baudRate)
    {
        case BAUD_115200:
            return SERIAL_BAUD_DIVISOR_115200;
        case BAUD_57600:
            return SERIAL_BAUD_DIVISOR_57600;
        case BAUD_38400:
            return SERIAL_BAUD_DIVISOR_38400;
        case BAUD_19200:
            return SERIAL_BAUD_DIVISOR_19200;
        case BAUD_9600:
            return SERIAL_BAUD_DIVISOR_9600;
        case BAUD_4800:
            return SERIAL_BAUD_DIVISOR_4800;
        case BAUD_2400:
            return SERIAL_BAUD_DIVISOR_2400;
        case BAUD_1200:
            return SERIAL_BAUD_DIVISOR_1200;
        case BAUD_300:
            return SERIAL_BAUD_DIVISOR_300;
        default:
        case BAUD_50:
            return SERIAL_BAUD_DIVISOR_50;
    }
    return SERIAL_BAUD_DIVISOR_9600; 
}

uint8_t Serial_GetDataBitsMask(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    switch (desc->mDataBits)
    {
        case 5:
            return SERIAL_DATA_BITS_5_MASK;
        case 6:
            return SERIAL_DATA_BITS_6_MASK;
        case 7:
            return SERIAL_DATA_BITS_7_MASK;
        case 8:
        default:
            return SERIAL_DATA_BITS_8_MASK;
    }
}

uint8_t Serial_GetStopBitsMask(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    switch (desc->mStopBits)
    {
        case 1:
            return SERIAL_STOP_BITS_1_MASK;
        case 2:
            return SERIAL_STOP_BITS_2_MASK;
        default:
            return 1;
    }
}

uint8_t Serial_GetParityMask(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    switch (desc->mParity)
    {
        case PARITY_None:
            return SERIAL_PARITY_NONE_MASK;
        case PARITY_Odd:
            return SERIAL_PARITY_ODD_MASK;
        case PARITY_Even:
            return SERIAL_PARITY_EVEN_MASK;
        case PARITY_Mark:
            return SERIAL_PARITY_MARK_MASK;
        case PARITY_Space:
            return SERIAL_PARITY_SPACE_MASK;
    }
}

// Register Manipulation Functions =============================================

uint8_t Serial_ReadInterruptIDRegister(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t iirAddress = baseAddress + SERIAL_INTERRUPT_ID_REGISTER_OFFSET;
    desc->mInterruptID = IO_ReadPort8b(iirAddress);
    if (self->Debug) printf("Serial: <- Port %d IIR: 0x%x\n",desc->mPortID, desc->mInterruptID);
}

void Serial_SetLineStatusRegister(struct Serial* self, volatile Serial_PortDescriptor* desc, uint8_t val)
{
    if (self->Debug) printf("Serial: -> Setting line status register 0x%x\n",val);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t lineStatusRegisterAddress = baseAddress + SERIAL_LINE_STATUS_REGISTER_OFFSET;
    IO_WritePort8b(lineStatusRegisterAddress,val);
}

void Serial_EnableDLAB(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) printf("Serial: -> Enabling DLAB on port %d\n",desc->mPortID);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, (currentLineControlValue | SERIAL_DLAB_ENABLE_MASK));
    currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (self->Debug) printf("Serial: <- LCR After enable DLAB 0x%x\n",currentLineControlValue);
}

void Serial_DisableDLAB(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) printf("Serial: -> Disabling DLAB on port %d\n",desc->mPortID);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    // Set DLAB disabled
    IO_WritePort8b(lineControlAddress, (currentLineControlValue & SERIAL_DLAB_DISABLE_MASK));
    currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (self->Debug) printf("Serial: <- LCR After disable DLAB 0x%x\n",currentLineControlValue);
}

void Serial_SetBaudRate(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) printf("Serial: -> Setting baud for port %d\n",desc->mPortID);
    Serial_EnableDLAB(self, desc);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t divisorLsbAddress = baseAddress + SERIAL_DLAB_DIVISOR_LSB_REGISTER_OFFSET;
    uint16_t divisorMsbAddress = baseAddress + SERIAL_DLAB_DIVISOR_MSB_REGISTER_OFFSET;
    uint16_t baudRateDivisor = Serial_GetDivisorFromBaudRate(self, desc);
    if (self->Debug) printf("Serial: -> Baud Divisor 0x%x\n",baudRateDivisor);
    // Send Divisor lsb
    IO_WritePort8b(divisorLsbAddress, baudRateDivisor & 0x00FF);
    IO_WritePort8b(divisorMsbAddress, baudRateDivisor >> 8);
    Serial_DisableDLAB(self, desc);
}

void Serial_SetDataBits(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) printf("Serial: -> Setting data bits for port %d\n",desc->mPortID);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetDataBitsMask(self, desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, ((currentLineControlValue & SERIAL_DATA_BITS_CLEAR_MASK) | mask));
    currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (self->Debug) printf("Serial: <- After data bits set 0x%x\n",currentLineControlValue);
}

void Serial_SetStopBits(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) printf("Serial: -> Setting stop bits for port %d\n",desc->mPortID);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetStopBitsMask(self, desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, ((currentLineControlValue & SERIAL_STOP_BITS_CLEAR_MASK) | mask));

    currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (self->Debug) printf("Serial: <- After stop bits set 0x%x\n",currentLineControlValue);
}

void Serial_SetParity(struct Serial* self,volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) printf("Serial: -> Setting parity for port %d\n",desc->mPortID);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetParityMask(self, desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, (currentLineControlValue & SERIAL_PARITY_CLEAR_MASK) | mask);
    currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (self->Debug) printf("Serial: <- After Parity set 0x%x\n", currentLineControlValue);
}

void Serial_SetInterruptEnableRegister(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) printf("Serial: -> Setting interrupts for port %d 0x%x\n",desc->mPortID,desc->mInterruptEnable);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t interruptRegisterAddress = baseAddress + SERIAL_INTERRUPT_REGISTER_OFFSET;
    IO_WritePort8b(interruptRegisterAddress, desc->mInterruptEnable);
    uint8_t val = IO_ReadPort8b(interruptRegisterAddress);
    if (self->Debug) printf("Serial: <- Port %d interrupts set 0x%x\n",desc->mPortID, val);
}

void Serial_ReadLineStatusRegister(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t lineStatusRegisterAddress = baseAddress + SERIAL_LINE_STATUS_REGISTER_OFFSET;
    uint8_t lineStatus = IO_ReadPort8b(lineStatusRegisterAddress);
    desc->mLineStatus = lineStatus;
}

void Serial_SetFifoControlRegister(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) printf("Serial: -> Set FIFO Control Register 0x%x\n",desc->mFifoControl);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t fifoAddress = baseAddress + SERIAL_FIFO_CTRL_REGISTER_OFFSET;
    IO_WritePort8b(fifoAddress, desc->mFifoControl);
}

void Serial_SetModemControlRegister(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) printf("Serial: -> Set Modem Control 0x%x\n",desc->mModemControl);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    uint16_t mcAddress = baseAddress + SERIAL_MODEM_CONTROL_REGISTER_OFFSET;
    IO_WritePort8b(mcAddress, desc->mModemControl);
    uint8_t val = IO_ReadPort8b(mcAddress);
    if (self->Debug) printf("Serial: <- After set modem control: 0x%x\n",val);
}

// Serial IO Functions ========================================================

uint8_t Serial_Read8b(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    while(!desc->mCanRead) {} 

    if (self->Debug) printf("Serial: <- Reading char from port %d\n", desc->mPortID);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    desc->mCanRead = 0;
    uint16_t data = IO_ReadPort8b(baseAddress);
    if (self->Debug) printf("Serial <- Read %c from port %d\n",data, desc->mPortID);
    return data;
}

void Serial_Write8b(struct Serial* self, volatile Serial_PortDescriptor* desc, uint8_t data)
{
    /* 
        TODO 
        * This does not seem to be set
        * Possible interrupt block while waiting, not 100% sure
    */
    while (!desc->mCanWrite) {}

    if (self->Debug) printf("Serial: -> Writing char on port %d\n", desc->mPortID);
    uint16_t baseAddress = self->PortAddresses[desc->mPortID-1];
    desc->mCanWrite = 0;
    IO_WritePort8b(baseAddress, data);
}

void Serial_WriteString(struct Serial* self, volatile Serial_PortDescriptor* desc, const char* string)
{
    if (self->Debug) printf("Serial: Writing string out on port %d...\n",desc->mPortID);
    while (*string)
    {
        Serial_Write8b(self, desc, *string);
        string++;
    }
}

// Debug Functions =============================================================

void Serial_DebugLineStatus(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) 
    {
        Serial_ReadLineStatusRegister(self, desc);
        printf("Serial: <- Current Line status: 0x%x\n",desc->mLineStatus);
        if ((desc->mLineStatus & SERIAL_STATUS_DATA_READY)!=0)
        {
            printf("\tData Ready\n");
        }
        if ((desc->mLineStatus & SERIAL_STATUS_OVERRUN_ERROR)!=0)               
        {
            printf("\tOverrun Error\n");
        }
        if ((desc->mLineStatus & SERIAL_STATUS_PARITY_ERROR)!=0)                
        {
            printf("\tParity Error\n");
        }
        if ((desc->mLineStatus & SERIAL_STATUS_FRAMING_ERROR)!=0)               
        {
            printf("\tFraming Error\n");
        }
        if ((desc->mLineStatus & SERIAL_STATUS_BREAK_INDICATOR)!=0)             
        {
            printf("\tBreak Indicator\n");
        }
        if ((desc->mLineStatus & SERIAL_STATUS_TRANSMITTER_HOLD_REG_EMPTY)!=0)  
        {
            printf("\tTransmit Holding register empty\n");
        }
        if ((desc->mLineStatus & SERIAL_STATUS_TRANSMITTER_EMPTY)!=0)           
        {
            printf("\tTransmit Empty\n");
        }
        if ((desc->mLineStatus & SERIAL_STATUS_IMPENDING_ERROR)!=0)             
        {
            printf("\tImpending Error\n");
        }
    }
}

void Serial_DebugInterruptID(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug)
    {
        Serial_ReadInterruptIDRegister(self, desc);
        printf("Serial: Current Interrupt ID Register 0x%x\n",desc->mInterruptID);
        if ((desc->mInterruptID & SERIAL_IIR_NO_INTERRUPT_PENDING) != 0)
        {
            printf("\tNo Interrupt Pending\n");
        }
        else
        {
            printf("\tInterrupt Pending\n");
        }
        
        if ((desc->mInterruptID & SERIAL_IIR_MODEM_STATUS_CHANGE) != 0)
        {
            printf("\tModem Status Changed\n");
        }

        if ((desc->mInterruptID & SERIAL_IIR_THRE) != 0)
        {
            printf("\tTHRE\n");
        }

        if ((desc->mInterruptID & SERIAL_IIR_RX_DATA_AVAILABLE) != 0)
        {
            printf("\tRX Data available\n");
        }

        if ((desc->mInterruptID & SERIAL_IIR_LINE_STATUS_CHANGE) != 0)
        {
            printf("\tLine Status Changed\n");
        }

        if ((desc->mInterruptID & SERIAL_IIR_CHAR_TIMEOUT) != 0)
        {
            printf("\tChar Timeout\n");
        }
    }
}

// Interrupt Handling ==========================================================

void Serial_SetupInterruptForPort1(struct Serial* self)
{
    if(self->Debug) printf("Serial: Setting up interrupt handler function for port 1\n");
	Interrupt_SetHandlerFunction(self->Interrupt, 4,Serial_Port1InterruptHandler);
}

void Serial_Port1InterruptHandler()
{
    struct Serial* self = &kernel.Serial;
    volatile Serial_PortDescriptor* desc = &Serial_Port1_8N1;
    if (self->Debug) 
    {
        printf("Serial: Interrupt on Port 1\n");
    }
    Serial_ReadLineStatusRegister(self, desc);
    Serial_ReadInterruptIDRegister(self, desc);

    if ((desc->mInterruptID & SERIAL_IIR_THRE) != 0) 
    {
        desc->mCanWrite = 1;
    }
    if ((desc->mInterruptID & SERIAL_IIR_RX_DATA_AVAILABLE) != 0)
    {
        desc->mCanRead = 1;
    }

    if (self->Debug) 
    {
        printf("Serial: Interrupt on port 1 done\n");
    }
}

// API Level Functions ========================================================

void Serial_SetupPort(struct Serial* self, volatile Serial_PortDescriptor* desc)
{
    if (self->Debug) printf("Serial: Setting up Port %d\n",desc->mPortID);

    // Stash and disable interrupts before setup
    uint8_t previousInterrupts = desc->mInterruptEnable;
    desc->mInterruptEnable = 0;
    Serial_SetInterruptEnableRegister(self, desc);

    Serial_SetBaudRate(self, desc);
    Serial_SetDataBits(self, desc);
    Serial_SetStopBits(self, desc);
    Serial_SetParity(self, desc);

    switch (desc->mPortID)
    {
        case 1:
            Serial_SetupInterruptForPort1(self);
            break;
        default:
            break;
    }

    Serial_SetFifoControlRegister(self, desc); 
    Serial_SetModemControlRegister(self, desc);

    // restore Interrupts
    desc->mInterruptEnable = previousInterrupts;
}

void Serial_TestPort1(struct Serial* self)
{
    char* test_str = "\n\nHello, World! From COM1\n\n";
    printf("Serial: Writing a test string to port %d\n", Serial_Port1_8N1.mPortID);
    Serial_WriteString(self, &Serial_Port1_8N1, test_str);
    /*
    int bMax = 64;
    int i;
    printf("Serial: Read %d bytes data\n\n",bMax);
    for (i=0; i<bMax; i++)
    {
        printf("%c",Serial_Read8b(&Serial_Port1_8N1));
    }
    */
}