#include "IO.h"

/* Output a byte to a port */
void IO_WritePort32b(uint16_t port, uint32_t value)
{
   asm volatile ("outl %%eax,%%dx": :"d" (port), "a" (value));
}

/* Output a byte to a port */
void IO_WritePort16b(uint16_t port, uint16_t value)
{
   asm volatile ("outw %%ax,%%dx": :"d" (port), "a" (value));
}

/* Output a byte to a port */
void IO_WritePort8b(uint16_t port, uint8_t value)
{
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

/* Input a byte from a port */
uint32_t IO_ReadPort32b(uint16_t port)
{
   uint32_t ret;
   asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
   return ret;
}

/* Input a byte from a port */
uint16_t IO_ReadPort16b(uint16_t port)
{
   uint16_t ret;
   asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port));
   return ret;
}

/* Input a byte from a port */
uint8_t IO_ReadPort8b(uint16_t port)
{
   uint8_t ret;
   asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
   return ret;
}

void IO_ReadPort32bString(uint32_t port, void *buffer, uint32_t count)
{
   asm volatile ("cld; rep; insl" :: "D" (buffer), "d" (port), "c" (count));
}