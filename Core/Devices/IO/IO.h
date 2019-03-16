#pragma once
#include <LibC/include/stdint.h>

void IO_WritePort(uint16_t port, uint8_t value);
uint8_t IO_ReadPort(uint16_t port);
void IO_WritePortLong(uint16_t port, uint32_t value);
uint32_t IO_ReadPortLong(uint16_t port);
