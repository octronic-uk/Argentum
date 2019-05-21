#pragma once

#include <stdint.h>

void FPUDriver_EnableFPU();
void FPUDriver_SetControlWord(const uint16_t cw);