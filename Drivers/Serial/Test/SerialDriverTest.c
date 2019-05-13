#include "SerialDriverTest.h"

#include <stdio.h>
#include <Drivers/Serial/SerialDriver.h>

void SerialDriverTest_RunSuite(SerialDriver* self)
{
    char* test_str = "\n\nHello, World! From COM1\n\n";
    printf("Serial: Writing a test string to port %d\n", SerialPort1_8N1.mPortID);
    SerialDriver_WriteString(self, &SerialPort1_8N1, test_str);
}