#include <stdio.h>

#include "FILE.h"
#include <Objects/Kernel/Kernel.h>
#include <Objects/HardwareManager/StorageManager/StorageManager.h>
#include <Drivers/Memory/MemoryDriver.h>

extern Kernel _Kernel;

FILE* fopen(const char* filename, const char* mode)
{
    printf("fopen %s\n",filename);
    FILE* f = StorageManager_RequestFilePointer(&_Kernel.HardwareManager.StorageManager);
    if (!f) return 0;

    if (!FILE_Constructor(f,filename, mode))
    {
        printf("fopen: FILE is null\n");
        StorageManager_CloseFilePointer(&_Kernel.HardwareManager.StorageManager,f);
        return 0;
    }
    if (!StorageManager_Open(&_Kernel.HardwareManager.StorageManager, f->DirectoryEntry, filename))
    {
        printf("fopen: FILE is no good\n");
        StorageManager_CloseFilePointer(&_Kernel.HardwareManager.StorageManager,f);
        return 0;
    }
    return f;
}