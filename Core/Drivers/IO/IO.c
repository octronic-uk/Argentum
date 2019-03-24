#include "IO.h"
#include <stdio.h>

uint8_t IO_Debug = 0;

void IO_SetDebug(uint8_t debug)
{
   IO_Debug = debug;
}

/* Output a byte to a port */
void IO_WritePort32b(uint16_t port, uint32_t value)
{
   if (IO_Debug) printf("IO: WritePort32b port 0x%x, value 0x%x\n",port,value);
   asm volatile ("outl %%eax,%%dx": :"d" (port), "a" (value));
}

/* Output a byte to a port */
void IO_WritePort16b(uint16_t port, uint16_t value)
{
   if (IO_Debug) printf("IO: WritePort16b port 0x%x, value 0x%x\n",port,value);
   asm volatile ("outw %%eax,%%dx": :"d" (port), "a" (value));
}

/* Output a byte to a port */
void IO_WritePort8b(uint16_t port, uint8_t value)
{
   if (IO_Debug) printf("IO: WritePort8b port 0x%x, value 0x%x\n",port,value);
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

/* Input a byte from a port */
uint32_t IO_ReadPort32b(uint16_t port)
{
   if (IO_Debug) printf("IO: ReadPortLong 0x%x \n",port);
   uint32_t ret;
   asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
   return ret;
}

/* Input a byte from a port */
uint16_t IO_ReadPort16b(uint16_t port)
{
   if (IO_Debug) printf("IO: ReadPortLong 0x%x \n",port);
   uint16_t ret;
   asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port));
   return ret;
}

/* Input a byte from a port */
uint8_t IO_ReadPort8b(uint16_t port)
{
   if (IO_Debug) printf("IO: ReadPort 0x%x\n",port);
   uint8_t ret;
   asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
   return ret;
}

void IO_ReadPort32bString(uint32_t port, void *buffer, uint32_t count)
{
   if (IO_Debug) printf("IO: ReadPort32bString port 0x%x, count %d\n",port,count);
   asm volatile ("cld; rep; insl" :: "D" (buffer), "d" (port), "c" (count));
}