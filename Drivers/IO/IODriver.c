#include <Drivers/IO/IODriver.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

bool IO_Debug = false;
bool _IO_ENABLE_SLEEP = false;

void IO_WritePort32b(uint16_t port, uint32_t value)
{
   if (IO_Debug) printf("IO: Write Port 32b Port: 0x%x, Value: 0x%x\n",port,value);
   asm volatile ("outl %%eax,%%dx": :"d" (port), "a" (value));
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
}

void IO_WritePort16b(uint16_t port, uint16_t value)
{
   if (IO_Debug) printf("IO: Write Port 16b Port: 0x%x, Value: 0x%x\n",port,value);
   asm volatile ("outw %%ax,%%dx": :"d" (port), "a" (value));
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
}

void IO_WritePort8b(uint16_t port, uint8_t value)
{
   if (IO_Debug) printf("IO: Write Port 8b Port: 0x%x, Value: 0x%x\n",port,value);
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
}

void IO_WritePort32bString(uint16_t port, void* buffer, uint32_t count)
{
   if (IO_Debug) printf("IO: Read Port 32b Port: 0x%x\n",port);
   asm volatile ("rep outsl"::"c"(count), "d"(port), "S"(buffer));
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
}

void IO_WritePort16bString(uint16_t port, void* buffer, uint32_t count)
{
   if (IO_Debug) printf("IO: Read Port 16b Port: 0x%x\n",port);
   asm volatile ("rep outsw"::"c"(count), "d"(port), "S"(buffer));
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
}

void IO_WritePort8bString(uint16_t port, void* buffer, uint32_t count)
{
   if (IO_Debug) printf("IO: Read Port 8b Port: 0x%x\n",port);
   asm volatile ("rep outsb"::"c"(count), "d"(port), "S"(buffer));
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
}


uint32_t IO_ReadPort32b(uint16_t port)
{
   if (IO_Debug) printf("IO: Read Port 32b Port: 0x%x\n",port);
   uint32_t ret;
   asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
   if(IO_Debug) printf("IO: Got 0x%x\n",ret);
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
   return ret;
}

uint16_t IO_ReadPort16b(uint16_t port)
{
   if (IO_Debug) printf("IO: Read Port 16b Port: 0x%x\n",port);
   uint16_t ret;
   asm volatile ( "inw %1, %0" : "=a"(ret) : "Nd"(port));
   if(IO_Debug) printf("IO: Got 0x%x\n",ret);
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
   return ret;
}

uint8_t IO_ReadPort8b(uint16_t port)
{
   if (IO_Debug) printf("IO: Read Port 8b Port: 0x%x\n",port);
   uint8_t ret;
   asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
   if(IO_Debug) printf("IO: Got 0x%x\n",ret);
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
   return ret;
}

void IO_ReadPort8bString(uint32_t port, void *buffer, uint32_t count)
{
   if (IO_Debug) printf("IO: Read String 8b Port: 0x%x, Count %d\n",port,count);
   asm volatile ("cld; rep; insb" :: "D" (buffer), "d" (port), "c" (count));
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
}

void IO_ReadPort16bString(uint32_t port, void *buffer, uint32_t count)
{
   if (IO_Debug) printf("IO: Read String 16b Port: 0x%x, Count %d\n",port,count);
   asm volatile ("cld; rep; insw" :: "D" (buffer), "d" (port), "c" (count));
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
}

void IO_ReadPort32bString(uint32_t port, void *buffer, uint32_t count)
{
   if (IO_Debug) printf("IO: Read String 32b Port: 0x%x, Count %d\n",port,count);
   asm volatile ("cld; rep; insl" :: "D" (buffer), "d" (port), "c" (count));
   if (_IO_ENABLE_SLEEP) usleep(IO_SLEEP);
}