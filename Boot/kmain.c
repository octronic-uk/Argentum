/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/

#include "../Kernel.h"
#include "../Core/Screen.h"

void kmain(void)
{
    const char* WelcomeString = "Taskie Version 1, Copyright Ash Thompson 2019 This is on some lines down";

    tkCoreClearScreen();
    tkCorePrint(WelcomeString);

    struct tkKernel* k = tkKernelAllocate();
    int retval = tkKernelExecute(k);
    tkKernelFree(k);

    return;
}

