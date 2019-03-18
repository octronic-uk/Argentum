#include "Serial.h"
#include <Devices/IO/IO.h>
#include <LibC/include/stdio.h>
#include <LibC/include/string.h>

uint16_t Serial_PortAddresses[4];

uint8_t Serial_Debug = 0;

void Serial_Constructor()
{
    printf("Serial: Constructing\n");
    char* test_str = "Testing Serial\n";
    Serial_SetPortAddressTable();
    if (Serial_Debug)
    {
        Serial_SetupPort(Serial_Port1_8N1);
        printf("Serial: Writing Test String %s\n", test_str);
        Serial_WriteString(Serial_Port1_8N1, test_str, strlen(test_str));
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

uint16_t Serial_GetDivisorFromBaudRate(uint32_t baudRate)
{
    switch(baudRate)
    {
        case 115200:
            return SERIAL_BAUD_DIVISOR_115200;
        case 57600:
            return SERIAL_BAUD_DIVISOR_57600;
        case 38400:
            return SERIAL_BAUD_DIVISOR_38400;
        case 28800:
            return SERIAL_BAUD_DIVISOR_28800;
        case 23040:
            return SERIAL_BAUD_DIVISOR_23040;
        case 19200:
            return SERIAL_BAUD_DIVISOR_19200;
        case 14400:
            return SERIAL_BAUD_DIVISOR_14400;
        case 12800:
            return SERIAL_BAUD_DIVISOR_12800;
        case 11520:
            return SERIAL_BAUD_DIVISOR_11520;
        default:
        case 9600:
            return SERIAL_BAUD_DIVISOR_9600;
    }
    return SERIAL_BAUD_DIVISOR_9600; 
}

void Serial_EnableDLAB(Serial_PortDescriptor desc)
{
    if (Serial_Debug) printf("Serial: Enabling DLAB on port %d\n",desc.mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc.mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, (currentLineControlValue | SERIAL_DLAB_ENABLE_MASK));
}

void Serial_DisableDLAB(Serial_PortDescriptor desc)
{
    if (Serial_Debug) printf("Serial: Disabling DLAB on port %d\n",desc.mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc.mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    // Set DLAB enabled
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, (currentLineControlValue & SERIAL_DLAB_DISABLE_MASK));
}

void Serial_SetBaudRate(Serial_PortDescriptor desc)
{
    if (Serial_Debug) printf("Serial: Setting baud for port %d\n",desc.mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc.mPortID-1];
    uint16_t divisorLsbAddress = baseAddress + SERIAL_DLAB_DIVISOR_LSB_REGISTER_OFFSET;
    uint16_t divisorMsbAddress = baseAddress + SERIAL_DLAB_DIVISOR_MSB_REGISTER_OFFSET;
    uint16_t baudRateDivisor = Serial_GetDivisorFromBaudRate(desc.mBaudRate);

    // Send Divisor lsb
    IO_WritePort8b(divisorLsbAddress, (uint8_t)(baudRateDivisor & 0x00FF));
    IO_WritePort8b(divisorMsbAddress, (uint8_t)((baudRateDivisor & 0xFF00)>>8));
}

void Serial_SetDataBits(Serial_PortDescriptor desc)
{
    if (Serial_Debug) printf("Serial: Setting data bits for port %d\n",desc.mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc.mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetDataBitsMask(desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, ((currentLineControlValue & SERIAL_DATA_BITS_CLEAR_MASK) | mask));
}

uint8_t Serial_GetDataBitsMask(Serial_PortDescriptor desc)
{
    switch (desc.mDataBits)
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

void Serial_SetStopBits(Serial_PortDescriptor desc)
{
    if (Serial_Debug) printf("Serial: Setting stop bits for port %d\n",desc.mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc.mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetStopBitsMask(desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, ((currentLineControlValue & SERIAL_STOP_BITS_CLEAR_MASK) | mask));
}

uint8_t Serial_GetStopBitsMask(Serial_PortDescriptor desc)
{
    switch (desc.mStopBits)
    {
        case 1:
            return SERIAL_STOP_BITS_1_MASK;
        case 2:
            return SERIAL_STOP_BITS_2_MASK;
        default:
            return 1;
    }
}

void Serial_SetParity(Serial_PortDescriptor desc)
{
    if (Serial_Debug) printf("Serial: Setting parity for port %d\n",desc.mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc.mPortID-1];
    uint16_t lineControlAddress = baseAddress + SERIAL_LINE_CONTROL_REGISTER_OFFSET;
    uint8_t mask = Serial_GetParityMask(desc);
    uint8_t currentLineControlValue = IO_ReadPort8b(lineControlAddress);
    IO_WritePort8b(lineControlAddress, ((currentLineControlValue & SERIAL_PARITY_CLEAR_MASK) | mask));

}

uint8_t Serial_GetParityMask(Serial_PortDescriptor desc)
{
    switch (desc.mParity)
    {
        case None:
            return SERIAL_PARITY_NONE_MASK;
        case Odd:
            return SERIAL_PARITY_ODD_MASK;
        case Even:
            return SERIAL_PARITY_EVEN_MASK;
        case Mark:
            return SERIAL_PARITY_MARK_MASK;
        case Space:
            return SERIAL_PARITY_SPACE_MASK;
    }
}


void Serial_SetInterruptRegister(Serial_PortDescriptor desc)
{
    if (Serial_Debug) printf("Serial: Setting interrupts for port %d\n",desc.mPortID);
    uint16_t baseAddress = Serial_PortAddresses[desc.mPortID-1];
    uint16_t interruptRegisterAddress = baseAddress + SERIAL_INTERRUPT_REGISTER_OFFSET;
    IO_WritePort8b(interruptRegisterAddress, desc.mInterrupts);
}

uint8_t Serial_ReadLineStatusRegister(Serial_PortDescriptor desc)
{
    uint16_t baseAddress = Serial_PortAddresses[desc.mPortID-1];
    uint16_t lineStatusRegisterAddress = baseAddress + SERIAL_LINE_STATUS_REGISTER_OFFSET;
    uint8_t lineStatus = IO_ReadPort8b(lineStatusRegisterAddress);
    return lineStatus;
}

uint8_t Serial_IsDataReady(Serial_PortDescriptor desc)
{
    return (Serial_ReadLineStatusRegister(desc) & SERIAL_STATUS_DATA_READY) != 0;
}

uint8_t Serial_Read8b(Serial_PortDescriptor desc)
{
    // Wait for data
    while (!Serial_IsDataReady(desc)) 
    {
        if (Serial_Debug)
        {
            printf("Serial: Waiting for port %d data ready\n",desc.mPortID);
        }
    }
    // Read
    uint8_t baseAddress = Serial_PortAddresses[desc.mPortID-1];
    return IO_ReadPort8b(baseAddress);
}

uint8_t Serial_IsTransmitEmpty(Serial_PortDescriptor desc)
{
    return (Serial_ReadLineStatusRegister(desc) & SERIAL_STATUS_TRANSMITTER_HOLD_REG_EMPTY) != 0;
}

void Serial_Write8b(Serial_PortDescriptor desc, uint8_t data)
{
    printf("Serial: Writing char on port %d... %c (0x%x)\n", desc.mPortID, data,data);
    // Wait for transmitter to empty
    while (!Serial_IsTransmitEmpty(desc)) 
    {
        if (Serial_Debug)
        {
            printf("Serial: Waiting for port %d transmit empty\n",desc.mPortID);
        }
    }
    // Write
    uint8_t baseAddress = Serial_PortAddresses[desc.mPortID-1];
    IO_WritePort8b(baseAddress, data);
}

void Serial_SetupPort(Serial_PortDescriptor desc)
{
    if (Serial_Debug) printf("Setting up Port %d\n",desc.mPortID);
    Serial_EnableDLAB(desc);
    Serial_SetBaudRate(desc);
    Serial_SetDataBits(desc);
    Serial_SetStopBits(desc);
    Serial_SetParity(desc);
    Serial_SetInterruptRegister(desc);
    Serial_DisableDLAB(desc);
}

void Serial_WriteString(Serial_PortDescriptor desc, const char* string, uint32_t len)
{
    printf("Serial: Writing String on port %d... %s\n",desc.mPortID, string);
    uint32_t i;
    for (i=0; i<len; i++)
    {
        Serial_Write8b(desc,string[i]);
    }
}
