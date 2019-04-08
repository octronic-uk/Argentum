#pragma once
#include <stdio.h>

/* This prints an "Assertion failed" message and aborts.  */
static inline void assert(const char *assertion, const char *filename, unsigned int linenumber, const char *function)
{
    printf("%s: %d: %s: Assertion `%s' failed.\n", 
        filename, linenumber, ((function == NULL) ? "?function?" : function), assertion);
    asm("cli");
    asm("hlt");
}
