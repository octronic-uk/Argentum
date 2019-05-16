#include <locale.h>
#include <stdio.h>
struct lconv* localeconv ()
{
    printf("localeconv\n");
    return 0;
}
