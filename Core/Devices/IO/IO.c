#include "IO.h"

/* Output a byte to a port */
void IO_WritePort(uint16_t port, uint8_t value)
{
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

/* Input a byte from a port */
uint8_t IO_ReadPort(uint16_t port)
{
   uint8_t ret;
   asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
   return ret;
}

/* Output a byte to a port */
void IO_WritePortLong(uint16_t port, uint32_t value)
{
   asm volatile ("outl %%eax,%%dx": :"d" (port), "a" (value));
}

/* Input a byte from a port */
uint32_t IO_ReadPortLong(uint16_t port)
{
   uint32_t ret;
   asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
   return ret;
}
