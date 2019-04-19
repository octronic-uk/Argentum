#include <stddef.h>
#include <string.h>

int string_n_compare(const char *s1, const char *s2, size_t n)
{
  unsigned char u1, u2;
  while (n-- > 0)
  {
    u1 = (unsigned char) *s1++;
    u2 = (unsigned char) *s2++;
    if (u1 != u2)
    {
      return u1 - u2;
    }
    if (u1 == '\0')
    {
      return 0;
    }
  }
  return 0;
}
