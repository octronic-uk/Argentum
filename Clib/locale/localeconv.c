#include <locale.h>

struct lconv* localeconv ()
{
    printf("stdlib: localeconv\n");
    return 0;
}
