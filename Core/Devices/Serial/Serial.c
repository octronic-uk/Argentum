#include "Serial.h"
#include <Devices/IO/IO.h>
#include <LibC/include/stdio.h>
#include <LibC/include/string.h>
#include <LibC/include/unistd.h>
#include <Devices/Interrupt/Interrupt.h>

uint16_t Serial_PortAddresses[4];
uint8_t Serial_Debug = 1;
Interrupt_DescriptorTableEntry Serial_Port1_IDTEntry;

void Serial_Constructor()
{
    uint16_t serial = IO_ReadPort16b(0x400);
    printf("Serial: Constructing (some port at 0x%x)\n");
    Serial_SetPortAddressTable();
    Serial_SetupPort(&Serial_Port1_8N1);
}

void Serial_TestPort1()
{
    if (Serial_Debug)
    {
        char* test_str = "Testing Serial\n";
        printf("Serial: Writing a test string to port %d\n", Serial_Port1_8N1.mPortID);
        Serial_WriteString(&Serial_Port1_8N1, test_str, strlen(test_str));
    }
}

void Serial_Destructor()
{
    printf("Serial: Destructing\n");
}

void Serial_SetPortAddressTable()
{
    if (Serial_Debug) printf("Serial: Setting Port Address Table\n");
    Serial_PortAddresses[0] = SERIAL_PORT_1_ADDRESS;
    Serial_PortAddresses[1] = SERIAL_PORT_2_ADDRESS;
    Serial_PortAddresses[2] = SERIAL_PORT_3_ADDRESS;
    Serial_PortAddresses[3] = SERIAL_PORT_4_ADDRESS;
}

uint16_t Serial_GetDivisorFromBaudRate(Serial_BaudRate baudRate)
{
    switch(baudRate)
    {
        case BAUD_115200:
            return SERIAL_BAUD_DIVISOR_115200;
        case BAUD_57600:
            return SERIAL_BAUD_DIVISOR_57600;
        case BAUD_38400:
            return SERIAL_BAUD_DIVISOR_38400;
        case BAUD_28800:
            return SERIAL_BAUD_DIVISOR_28800;
        case BAUD_23040:
            return SERIAL_BAUD_DIVISOR_23040;
        case BAUD_19200:
            return SERIAL_BAUD_DIVISOR_19200;
        case BAUD_14400:
            return SERIAL_BAUD_DIVISOR_14400;
        case BAUD_12800:
            return SERIAL_BAUD_DIVISOR_12800;
        case BAUD_11520:
            return SERIAL_BAUD_DIVISOR_11520;
        default:
        case BAUD_9600:
            return SERIAL_BAUD_DIVISOR_9600;
    }
    return SERIAL_BAUD_DIVISOR_9600; 
}

void Serial_EnableDLAB(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Enabling DLAB on port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (Serial_Debug) printf("Serial: LineControlValue 0x%x\n",currentLineControlValue);
    IO_WritePort8b(lineControlAddress, (currentLineControlValue | SERIAL_DLAB_ENABLE_MASK));
    if (Serial_Debug)
    {
        uint8_t currentLineControlValueAfter = IO_ReadPort8b(lineControlAddress);
        printf("Serial: LineControlValue After 0x%x\n",currentLineControlValueAfter);
    }

}

void Serial_DisableDLAB(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Disabling DLAB on port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    // Set DLAB enabled
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (Serial_Debug) printf("Serial: LineControlValue 0x%x\n",currentLineControlValue);
    IO_WritePort8b(lineControlAddress, (currentLineControlValue & SERIAL_DLAB_DISABLE_MASK));
    if (Serial_Debug)
    {
        uint8_t currentLineControlValueAfter = IO_ReadPort8b(lineControlAddress);
        printf("Serial: LineControlValue After 0x%x\n",currentLineControlValueAfter);
    }
}

void Serial_SetBaudRate(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Setting baud for port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t divisorLsbAddress = baseAddress + SERIAL_DLAB_DIVISOR_LSB_REGISTER_OFFSET;
    uint16_t divisorMsbAddress = baseAddress + SERIAL_DLAB_DIVISOR_MSB_REGISTER_OFFSET;
    uint16_t baudRateDivisor = Serial_GetDivisorFromBaudRate(desc->mBaudRate);
    if (Serial_Debug) printf("Serial: Baud Divisor 0x%x\n",baudRateDivisor);

    // Send Divisor lsb
    IO_WritePort8b(divisorLsbAddress, baudRateDivisor & 0x00FF);
    IO_WritePort8b(divisorMsbAddress, baudRateDivisor >> 8);
    if (Serial_Debug)
    {
        printf("Serial: Baud Divisor Now lsb.0x%x msb.0x%x\n",IO_ReadPort8b(divisorLsbAddress),IO_ReadPort8b(divisorMsbAddress));
    }
}

void Serial_SetDataBits(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Setting data bits for port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetDataBitsMask(desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (Serial_Debug) printf("Serial: LineControlValue 0x%x\n",currentLineControlValue);
    IO_WritePort8b(lineControlAddress, ((currentLineControlValue & SERIAL_DATA_BITS_CLEAR_MASK) | mask));
    if (Serial_Debug)
    {
        uint8_t currentLineControlValueAfter = IO_ReadPort8b(lineControlAddress);
        printf("Serial: LineControlValue After 0x%x\n",currentLineControlValueAfter);
    }
}

uint8_t Serial_GetDataBitsMask(Serial_PortDescriptor* desc)
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

void Serial_SetStopBits(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Setting stop bits for port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetStopBitsMask(desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (Serial_Debug) printf("Serial: LineControlValue 0x%x\n",currentLineControlValue);
    IO_WritePort8b(lineControlAddress, ((currentLineControlValue & SERIAL_STOP_BITS_CLEAR_MASK) | mask));
    if (Serial_Debug)
    {
        uint8_t currentLineControlValueAfter = IO_ReadPort8b(lineControlAddress);
        printf("Serial: LineControlValue After 0x%x\n",currentLineControlValueAfter);
    }
}

uint8_t Serial_GetStopBitsMask(Serial_PortDescriptor* desc)
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

void Serial_SetParity(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Setting parity for port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetParityMask(desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (Serial_Debug) printf("Serial: LineControlValue 0x%x\n", currentLineControlValue);
    IO_WritePort8b(lineControlAddress, (currentLineControlValue & SERIAL_PARITY_CLEAR_MASK) | mask);
    if (Serial_Debug)
    {
        uint8_t currentLineControlValueAfter = IO_ReadPort8b(lineControlAddress);
        printf("Serial: LineControlValue After 0x%x\n",currentLineControlValueAfter);
    }

}

uint8_t Serial_GetParityMask(Serial_PortDescriptor* desc)
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

void Serial_SetInterruptEnableRegister(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Setting interrupts for port %d 0x%x\n",desc->mPortID,desc->mInterruptEnable);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t interruptRegisterAddress = baseAddress + SERIAL_INTERRUPT_REGISTER_OFFSET;
    IO_WritePort8b(interruptRegisterAddress, desc->mInterruptEnable);
}

void Serial_ReadLineStatusRegister(Serial_PortDescriptor* desc)
{
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineStatusRegisterAddress = baseAddress + SERIAL_LINE_STATUS_REGISTER_OFFSET;
    uint8_t lineStatus = IO_ReadPort8b(lineStatusRegisterAddress);
    if (Serial_Debug) printf("Serial: Got line status 0x%x\n",lineStatus);
    desc->mStatus = lineStatus;
}

uint8_t Serial_Read8b(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Reading char from port %d\n", desc->mPortID);
    uint8_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint8_t data = IO_ReadPort8b(baseAddress);
    desc->mInputBuffer[desc->mInputBufferWriteIndex++] = data;
    if (desc->mInputBufferWriteIndex > SERIAL_BUFFER_SIZE) desc->mInputBufferWriteIndex = 0;
    return data;
}

void Serial_Write8b(Serial_PortDescriptor* desc)
{
    if (desc->mOutputBytesToWrite)
    {
        if (Serial_Debug) printf("Serial: Writing char on port %d from index %d\n", desc->mPortID,desc->mOutputBufferReadIndex);
        uint8_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
        IO_WritePort8b(baseAddress, desc->mOutputBuffer[desc->mOutputBufferReadIndex++]);
        if (desc->mOutputBufferReadIndex > SERIAL_BUFFER_SIZE) desc->mOutputBufferReadIndex = 0;
        desc->mOutputBytesToWrite--;
        if (Serial_Debug) printf("Serial: Port %d has %d bytes left\n", desc->mPortID,desc->mOutputBytesToWrite);
    }
    else
    {
        if (Serial_Debug) printf("Serial: Port %d No output bytes to write\n", desc->mPortID);
    }
}

void Serial_SetupPort(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Setting up Port %d\n",desc->mPortID);
    // Disable Interrupts first
    Serial_SetInterruptEnableRegister(desc);
    usleep(20);

    Serial_EnableDLAB(desc);
    usleep(20);
    Serial_SetBaudRate(desc);
    usleep(20);
    Serial_DisableDLAB(desc);
    usleep(20);

    Serial_SetDataBits(desc);
    usleep(20);
    Serial_SetStopBits(desc);
    usleep(20);
    Serial_SetParity(desc);
    usleep(20);

    switch (desc->mPortID)
    {
        case 1:
            Serial_SetupInterruptForPort1();
            usleep(20);
            break;
        default:
            break;
    }

    Serial_SetIDFIFORegister(desc); 
    usleep(20);
    Serial_SetModemControlRegister(desc);
    usleep(20);

    // Enable Interrupts
    desc->mInterruptEnable = 0x02;
    Serial_SetInterruptEnableRegister(desc);
    usleep(20);
}

uint8_t Serial_WriteString(Serial_PortDescriptor* desc, const char* string, uint32_t len)
{
    if (Serial_Debug) printf("Serial: Writing string out on port %d...\n",desc->mPortID);
    uint32_t maxOut = 0;
    uint32_t bytesOut = 0;
    switch (desc->mPortID)
    {
        case 1:
            // Get max buffer space available
            maxOut = SERIAL_BUFFER_SIZE - desc->mOutputBufferWriteIndex;
            bytesOut = len > maxOut ? maxOut : len;
            memcpy(&desc->mOutputBuffer[desc->mOutputBufferWriteIndex],string,bytesOut);
            desc->mOutputBufferWriteIndex += bytesOut;
            if (desc->mOutputBufferWriteIndex > SERIAL_BUFFER_SIZE) 
            {
                desc->mOutputBufferWriteIndex = 0;
            }
            printf("\tBuffered %d/%d bytes\n",bytesOut);
            desc->mOutputBytesToWrite += bytesOut;
            Serial_Write8b(desc);
            return bytesOut;
        break;
    }
    return 0;
}

void Serial_CheckStatus(Serial_PortDescriptor* desc)
{
    Serial_ReadLineStatusRegister(desc);
    printf("Serial: Check status: 0x%x\n",desc->mStatus);
    if ((desc->mStatus & SERIAL_STATUS_DATA_READY)!=0)                  printf("\tData Ready\n");
    if ((desc->mStatus & SERIAL_STATUS_OVERRUN_ERROR)!=0)               printf("\tOverrun Error\n");
    if ((desc->mStatus & SERIAL_STATUS_PARITY_ERROR)!=0)                printf("\tParity Error\n");
    if ((desc->mStatus & SERIAL_STATUS_FRAMING_ERROR)!=0)               printf("\tFraming Error\n");
    if ((desc->mStatus & SERIAL_STATUS_BREAK_INDICATOR)!=0)             printf("\tBreak Indicator\n");
    if ((desc->mStatus & SERIAL_STATUS_TRANSMITTER_HOLD_REG_EMPTY)!=0)  printf("\tTransmit Holding register empty\n");
    if ((desc->mStatus & SERIAL_STATUS_TRANSMITTER_EMPTY)!=0)           printf("\tTransmit Empty\n");
    if ((desc->mStatus & SERIAL_STATUS_IMPENDING_ERROR)!=0)             printf("\tImpending doom!\n");
}

void Serial_SetIDFIFORegister(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Set ID/FIFO 0x%x\n",desc->mIDFifo);
    uint8_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint8_t fifoAddress = baseAddress + SERIAL_INTERRUPT_ID_FIFO_REGISTER_OFFSET;
    IO_WritePort8b(fifoAddress, desc->mIDFifo);
}

void Serial_SetModemControlRegister(Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Set Modem Control 0x%x\n",desc->mModemControl);
    uint8_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint8_t mcAddress = baseAddress + SERIAL_MODEM_CONTROL_REGISTER_OFFSET;
    IO_WritePort8b(mcAddress, desc->mModemControl);
}

void Serial_Port1InterruptHandler()
{
    printf("Serial: Interrupt on Port 1\n");
	Interrupt_SendEOI_PIC1();
    Serial_CheckStatus(&Serial_Port1_8N1);

    if ((Serial_Port1_8N1.mStatus & SERIAL_STATUS_DATA_READY) != 0) 
    {
        Serial_Read8b(&Serial_Port1_8N1);
    }
    else if ((Serial_Port1_8N1.mStatus & SERIAL_STATUS_TRANSMITTER_EMPTY) != 0) 
    {
        Serial_Write8b(&Serial_Port1_8N1);
    }
    printf("Serial: Interrupt on port 1 done\n");
}

void Serial_SetupInterruptForPort1()
{
    if(Serial_Debug) printf("Serial: Setting up interrupt handler function for port 1\n");
    memset(&Serial_Port1_IDTEntry,0,sizeof(Interrupt_DescriptorTableEntry));
    uint32_t serial_port1_asm = (uint32_t)Serial_Port1InterruptHandlerASM;
	Serial_Port1_IDTEntry.mOffsetLowerBits = serial_port1_asm & 0xffff;
	Serial_Port1_IDTEntry.mOffsetHigherBits = (serial_port1_asm & 0xffff0000) >> 16;
	Serial_Port1_IDTEntry.mSelector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Serial_Port1_IDTEntry.mZero = 0;
	Serial_Port1_IDTEntry.mTypeAttribute = INTERRUPT_GATE;
	Interrupt_SetIDTEntry(SERIAL_PORT_1_IDT_INDEX,Serial_Port1_IDTEntry);
}