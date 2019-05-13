#pragma once
#include <stdio.h>
#include <stddef.h>

#define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__, __func__),0)))

/* This prints an "Assertion failed" message and aborts.  */
static inline void __assert_fail(const char *assertion, const char *filename, unsigned int linenumber, const char *function)
{
    printf("%s: %d: %s: Assertion `%s' failed.\n", filename, linenumber, ((function == NULL) ? "?function?" : function), assertion);
    asm("cli");
    asm("hlt");
}
