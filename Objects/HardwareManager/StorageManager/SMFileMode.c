#include "SMFileMode.h"

#include <string.h>
#include <stdio.h>

bool SMFileMode_Constructor(SMFileMode* self, const char* mode)
{
    printf("SMFileMode: Constructor\n");
    memset(self,0,sizeof(SMFileMode));

    char* ch = (char*)mode;

    while (*ch)
    {
        switch (*ch)
        {
            case SM_FILEMODE_APPEND:
                self->Append = true;
                break;
            case SM_FILEMODE_BINARY:
                self->Binary = true;
                break;
            case SM_FILEMODE_READ:
                self->Read = true;
                break;
            case SM_FILEMODE_UPDATE:
                self->Update = true;
                break;
            case SM_FILEMODE_WRITE:
                self->Write = true;
                break;
            default:
                return false;
        }
        ch++;
    }
    return true;
}