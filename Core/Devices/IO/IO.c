#include "IO.h"
#include <LibC/include/stdio.h>

uint8_t IO_Debug = 0;

void IO_SetDebug(uint8_t debug)
{
   IO_Debug = debug;
}

/* Output a byte to a port */
void IO_WritePort(uint16_t port, uint8_t value)
{
   if (IO_Debug) printf("IO: WritePort port 0x%x, value 0x%x\n",port,value);
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

/* Input a byte from a port */
uint8_t IO_ReadPort(uint16_t port)
{
   if (IO_Debug) printf("IO: ReadPort 0x%x\n",port);
   uint8_t ret;
   asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
   return ret;
}

/* Output a byte to a port */
void IO_WritePortLong(uint16_t port, uint32_t value)
{
   if (IO_Debug) printf("IO: WritePortLong 0x%x, 0x%x\n",port,value);
   asm volatile ("outl %%eax,%%dx": :"d" (port), "a" (value));
}

/* Input a byte from a port */
uint32_t IO_ReadPortLong(uint16_t port)
{
   if (IO_Debug) printf("IO: ReadPortLong 0x%x \n",port);
   uint32_t ret;
   asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
   return ret;
}

void IO_ReadPortLongString(uint32_t port, void *dst, uint32_t count)
{
   if (IO_Debug) printf("IO: ReadPortLongString port 0x%x, count %d\n",port,count);
	uint32_t *dp = dst;

	while (count--)
   {
		*dp++ = IO_ReadPortLong(port);
   }
}