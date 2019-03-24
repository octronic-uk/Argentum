#include "Serial.h"
#include <Drivers/IO/IO.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <Drivers/Interrupt/Interrupt.h>

uint16_t Serial_PortAddresses[4];
uint8_t Serial_Debug = 0;
Interrupt_DescriptorTableEntry Serial_Port1_IDTEntry;

void Serial_Constructor()
{
    printf("Serial: Constructing\n");
    Serial_SetPortAddressTable();
    Serial_SetupPort(&Serial_Port1_8N1);
}

void Serial_Destructor()
{
    printf("Serial: Destructing\n");
}

// Helper Functions ============================================================

void Serial_SetPortAddressTable()
{
    if (Serial_Debug) printf("Serial: Setting Port Address Table\n");
    Serial_PortAddresses[0] = SERIAL_PORT_1_ADDRESS;
    Serial_PortAddresses[1] = SERIAL_PORT_2_ADDRESS;
    Serial_PortAddresses[2] = SERIAL_PORT_3_ADDRESS;
    Serial_PortAddresses[3] = SERIAL_PORT_4_ADDRESS;
}

uint16_t Serial_GetDivisorFromBaudRate(volatile Serial_PortDescriptor* desc)
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

uint8_t Serial_GetDataBitsMask(volatile Serial_PortDescriptor* desc)
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

uint8_t Serial_GetStopBitsMask(volatile Serial_PortDescriptor* desc)
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

uint8_t Serial_GetParityMask(volatile Serial_PortDescriptor* desc)
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

uint8_t Serial_ReadInterruptIDRegister(volatile Serial_PortDescriptor* desc)
{
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t iirAddress = baseAddress + SERIAL_INTERRUPT_ID_REGISTER_OFFSET;
    desc->mInterruptID = IO_ReadPort8b(iirAddress);
    if (Serial_Debug) printf("Serial: <- Port %d IIR: 0x%x\n",desc->mPortID, desc->mInterruptID);
}

void Serial_SetLineStatusRegister(volatile Serial_PortDescriptor* desc, uint8_t val)
{
    if (Serial_Debug) printf("Serial: -> Setting line status register 0x%x\n",val);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineStatusRegisterAddress = baseAddress + SERIAL_LINE_STATUS_REGISTER_OFFSET;
    IO_WritePort8b(lineStatusRegisterAddress,val);
}

void Serial_EnableDLAB(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: -> Enabling DLAB on port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, (currentLineControlValue | SERIAL_DLAB_ENABLE_MASK));
    currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (Serial_Debug) printf("Serial: <- LCR After enable DLAB 0x%x\n",currentLineControlValue);
}

void Serial_DisableDLAB(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: -> Disabling DLAB on port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    // Set DLAB disabled
    IO_WritePort8b(lineControlAddress, (currentLineControlValue & SERIAL_DLAB_DISABLE_MASK));
    currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (Serial_Debug) printf("Serial: <- LCR After disable DLAB 0x%x\n",currentLineControlValue);
}

void Serial_SetBaudRate(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: -> Setting baud for port %d\n",desc->mPortID);
    Serial_EnableDLAB(desc);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t divisorLsbAddress = baseAddress + SERIAL_DLAB_DIVISOR_LSB_REGISTER_OFFSET;
    uint16_t divisorMsbAddress = baseAddress + SERIAL_DLAB_DIVISOR_MSB_REGISTER_OFFSET;
    uint16_t baudRateDivisor = Serial_GetDivisorFromBaudRate(desc);
    if (Serial_Debug) printf("Serial: -> Baud Divisor 0x%x\n",baudRateDivisor);
    // Send Divisor lsb
    IO_WritePort8b(divisorLsbAddress, baudRateDivisor & 0x00FF);
    IO_WritePort8b(divisorMsbAddress, baudRateDivisor >> 8);
    Serial_DisableDLAB(desc);
}

void Serial_SetDataBits(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: -> Setting data bits for port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetDataBitsMask(desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, ((currentLineControlValue & SERIAL_DATA_BITS_CLEAR_MASK) | mask));
    currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (Serial_Debug) printf("Serial: <- After data bits set 0x%x\n",currentLineControlValue);
}

void Serial_SetStopBits(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: -> Setting stop bits for port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetStopBitsMask(desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, ((currentLineControlValue & SERIAL_STOP_BITS_CLEAR_MASK) | mask));

    currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (Serial_Debug) printf("Serial: <- After stop bits set 0x%x\n",currentLineControlValue);
}

void Serial_SetParity(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: -> Setting parity for port %d\n",desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetParityMask(desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, (currentLineControlValue & SERIAL_PARITY_CLEAR_MASK) | mask);
    currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    if (Serial_Debug) printf("Serial: <- After Parity set 0x%x\n", currentLineControlValue);
}

void Serial_SetInterruptEnableRegister(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: -> Setting interrupts for port %d 0x%x\n",desc->mPortID,desc->mInterruptEnable);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t interruptRegisterAddress = baseAddress + SERIAL_INTERRUPT_REGISTER_OFFSET;
    IO_WritePort8b(interruptRegisterAddress, desc->mInterruptEnable);
    uint8_t val = IO_ReadPort8b(interruptRegisterAddress);
    if (Serial_Debug) printf("Serial: <- Port %d interrupts set 0x%x\n",desc->mPortID, val);
}

void Serial_ReadLineStatusRegister(volatile Serial_PortDescriptor* desc)
{
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t lineStatusRegisterAddress = baseAddress + SERIAL_LINE_STATUS_REGISTER_OFFSET;
    uint8_t lineStatus = IO_ReadPort8b(lineStatusRegisterAddress);
    desc->mLineStatus = lineStatus;
}

void Serial_SetFifoControlRegister(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: -> Set FIFO Control Register 0x%x\n",desc->mFifoControl);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t fifoAddress = baseAddress + SERIAL_FIFO_CTRL_REGISTER_OFFSET;
    IO_WritePort8b(fifoAddress, desc->mFifoControl);
}

void Serial_SetModemControlRegister(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: -> Set Modem Control 0x%x\n",desc->mModemControl);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    uint16_t mcAddress = baseAddress + SERIAL_MODEM_CONTROL_REGISTER_OFFSET;
    IO_WritePort8b(mcAddress, desc->mModemControl);
    uint8_t val = IO_ReadPort8b(mcAddress);
    if (Serial_Debug) printf("Serial: <- After set modem control: 0x%x\n",val);
}

// Serial IO Functions ========================================================

uint8_t Serial_Read8b(volatile Serial_PortDescriptor* desc)
{
    while(!desc->mCanRead) {} 

    if (Serial_Debug) printf("Serial: <- Reading char from port %d\n", desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    desc->mCanRead = 0;
    uint16_t data = IO_ReadPort8b(baseAddress);
    //desc->mFifoControl = 0x03;
    //Serial_SetFifoControlRegister(desc);
    if (Serial_Debug) printf("Serial <- Read %c from port %d\n",data, desc->mPortID);
    return data;
}

void Serial_Write8b(volatile Serial_PortDescriptor* desc, uint8_t data)
{
    while (!desc->mCanWrite) if (Serial_Debug) printf("Waiting for write = 1\n");

    if (Serial_Debug) printf("Serial: -> Writing char on port %d\n", desc->mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc->mPortID-1];
    desc->mCanWrite = 0;
    IO_WritePort8b(baseAddress, data);
    //desc->mFifoControl = 0x05;
    //Serial_SetFifoControlRegister(desc);
}

void Serial_WriteString(volatile Serial_PortDescriptor* desc, const char* string)
{
    if (Serial_Debug) printf("Serial: Writing string out on port %d...\n",desc->mPortID);
    switch (desc->mPortID)
    {
        case 1:
            while (*string)
            {
                Serial_Write8b(desc, *string);
                string++;
            }
        break;
    }
}

// Debug Functions =============================================================

void Serial_DebugLineStatus(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) 
    {
        Serial_ReadLineStatusRegister(desc);
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

void Serial_DebugInterruptID(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug)
    {
        Serial_ReadInterruptIDRegister(desc);
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

void Serial_Port1InterruptHandler()
{
    volatile Serial_PortDescriptor* desc = &Serial_Port1_8N1;
    if (Serial_Debug) printf("Serial: Interrupt on Port 1\n");
    Serial_ReadLineStatusRegister(desc);
    Serial_ReadInterruptIDRegister(desc);

    if ((desc->mInterruptID & SERIAL_IIR_THRE) != 0) 
    {
        desc->mCanWrite = 1;
    }
    if ((desc->mInterruptID & SERIAL_IIR_RX_DATA_AVAILABLE) != 0)
    {
        desc->mCanRead = 1;
    }

    if (Serial_Debug) printf("Serial: Interrupt on port 1 done\n");
	Interrupt_SendEOI_PIC1();
}

// API Level Functions ========================================================

void Serial_SetupPort(volatile Serial_PortDescriptor* desc)
{
    if (Serial_Debug) printf("Serial: Setting up Port %d\n",desc->mPortID);

    // Stash and disable interrupts before setup
    uint8_t previousInterrupts = desc->mInterruptEnable;
    desc->mInterruptEnable = 0;
    Serial_SetInterruptEnableRegister(desc);

    Serial_SetBaudRate(desc);
    Serial_SetDataBits(desc);
    Serial_SetStopBits(desc);
    Serial_SetParity(desc);

    switch (desc->mPortID)
    {
        case 1:
            Serial_SetupInterruptForPort1();
            break;
        default:
            break;
    }

    Serial_SetFifoControlRegister(desc); 
    Serial_SetModemControlRegister(desc);

    // restore Interrupts
    desc->mInterruptEnable = previousInterrupts;
}

void Serial_TestPort1()
{
    char* test_str = "Hello, World!\n";
    printf("Serial: Writing a test string to port %d\n", Serial_Port1_8N1.mPortID);
    Serial_WriteString(&Serial_Port1_8N1, test_str);
    int bMax = 64;
    int i;
    printf("Serial: Read %d bytes data\n\n",bMax);
    for (i=0; i<bMax; i++)
    {
        printf("%c",Serial_Read8b(&Serial_Port1_8N1));
    }
}