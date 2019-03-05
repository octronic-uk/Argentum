/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/

#include "../Kernel.h"
#include "../Core/Screen/Screen.h"

void kmain(void)
{
    tkScreenClear();
    tkScreenPrintLine("====[ Taskie Version 1 - 5/3/19 ]====\n    Copyright Ash Thompson 2019\n\n");

    struct tkKernel* k = tkKernelAllocate();
    int retval = tkKernelExecute(k);
    tkKernelFree(k);

    return;
}

