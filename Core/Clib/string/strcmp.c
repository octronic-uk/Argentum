#include <stddef.h>
#include <string.h>

/* Assumes zero terminated strings */
int string_compare(const char *s1, const char *s2)
{
  unsigned char u1, u2;
  int i=0;
  while (s1[i] && s2[i])
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
