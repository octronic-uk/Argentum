#pragma once
#include "../Types.h"

void tkIO_WritePort(uint16_t port, uint8_t value);
uint8_t tkIO_ReadPort(uint16_t port);

void tkIO_WritePortLong(uint16_t port, uint32_t value);
uint32_t tkIO_ReadPortLong(uint16_t port);

