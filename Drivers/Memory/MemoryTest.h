#pragma once

#include "MemoryDriver.h"

void MemoryTest_Allocate(struct MemoryDriver* self) ;
void MemoryTest_ReallocateCaseA(struct MemoryDriver* self) ;
void MemoryTest_ReallocateCaseB(struct MemoryDriver* self) ;
void MemoryTest_Free(struct MemoryDriver* self) ;
void MemoryTest_RunSuite(struct MemoryDriver* self);