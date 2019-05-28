#pragma once

struct MemoryDriver;

void MemoryDriverTest_Allocate(struct MemoryDriver* self) ;
void MemoryDriverTest_ReallocateCaseA(struct MemoryDriver* self) ;
void MemoryDriverTest_ReallocateCaseB(struct MemoryDriver* self) ;
void MemoryDriverTest_Free(struct MemoryDriver* self) ;
void MemoryDriverTest_RunSuite(struct MemoryDriver* self);