#include <stdio.h>

#include "FILE.h"
#include <Objects/Kernel/Kernel.h>
#include <Objects/StorageManager/StorageManager.h>
#include <Drivers/Memory/MemoryDriver.h>

extern Kernel _Kernel;

FILE* fopen(const char* filename, const char* mode)
{
    printf("stdlib: fopen \n");
    FILE* f = StorageManager_RequestFilePointer(&_Kernel.StorageManager);
    if (!FILE_Constructor(f,filename, mode))
    {
        StorageManager_CloseFilePointer(&_Kernel.StorageManager,f);
        return 0;
    }
    if (!StorageManager_Open(&_Kernel.StorageManager, f->DirectoryEntry, filename))
    {
        StorageManager_CloseFilePointer(&_Kernel.StorageManager,f);
        return 0;
    }
    return f;
}