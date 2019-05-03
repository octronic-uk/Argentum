#include <locale.h>
#include <stdio.h>
struct lconv* localeconv ()
{
    printf("stdlib: localeconv\n");
    return 0;
}
