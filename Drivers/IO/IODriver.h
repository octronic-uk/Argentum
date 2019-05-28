#pragma once
#include <stdint.h>

#define IO_SLEEP 1 

void IO_WritePort32b(uint16_t port, uint32_t value);
void IO_WritePort16b(uint16_t port, uint16_t value);
void IO_WritePort8b(uint16_t port, uint8_t value);

void IO_WritePort32bString(uint16_t port, void* buffer, uint32_t count);
void IO_WritePort16bString(uint16_t port, void* buffer, uint32_t count);
void IO_WritePort8bString(uint16_t port, void* buffer, uint32_t count);

uint32_t IO_ReadPort32b(uint16_t port);
uint16_t IO_ReadPort16b(uint16_t port);
uint8_t  IO_ReadPort8b(uint16_t port);

void IO_ReadPort8bString(uint32_t port, void *dst, uint32_t count);
void IO_ReadPort16bString(uint32_t port, void *dst, uint32_t count);
void IO_ReadPort32bString(uint32_t port, void *dst, uint32_t count);