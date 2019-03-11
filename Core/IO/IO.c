#include "IO.h"

/* Output a byte to a port */
void tkIO_WritePort(uint16_t port, uint8_t value)
{
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

/* Input a byte from a port */
uint8_t tkIO_ReadPort(uint16_t port)
{
   uint8_t ret;
   asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
   return ret;
}

/* Output a byte to a port */
void tkIO_WritePortLong(uint16_t port, uint32_t value)
{
   asm volatile ("out %%al,%%dx": :"d" (port), "a" (value));
}

/* Input a byte from a port */
uint32_t tkIO_ReadPortLong(uint16_t port)
{
   uint32_t ret;
   asm volatile ( "in %1, %0" : "=a"(ret) : "Nd"(port) );
   return ret;
}